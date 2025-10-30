import asyncio
import inspect
import logging
import os
import pathlib
import shlex
import time
import hashlib
import atexit
import json
import re
import sys

logger = logging.getLogger(__name__)

shared_semaphore = None

trace_entries = []
LAST_BUILD_TIMES = {}
ALREADY_RUN = {}
_last_build_times = pathlib.Path("last_build_times.json")
if _last_build_times.exists():
    with open(_last_build_times) as f:
        LAST_BUILD_TIMES = json.load(f)
    logger.info("Build times loaded.")
else:
    logger.warn(
        "No last build times found. This is normal if you're running this for the first time."
    )


def save_trace():
    with open("trace.json", "w") as f:
        json.dump(trace_entries, f)
    with open("last_build_times.json", "w") as f:
        json.dump(LAST_BUILD_TIMES, f)
    logger.info("wrote trace %s", pathlib.Path(".").absolute() / "trace.json")
    logger.info("wrote build times %s", pathlib.Path(".").absolute() / "last_build_times.json")


atexit.register(save_trace)


class _TokenProtocol(asyncio.Protocol):
    def __init__(self, client):
        self.client = client

    def data_received(self, data):
        # Data can be multiple tokens at once.
        for i, _ in enumerate(data):
            self.client.new_token(data[i : i + 1])


class _MakeJobClient:
    def __init__(self, fifo_path=None, read_fd=None, write_fd=None):
        self.fifo_path = fifo_path
        if fifo_path is not None:
            self.writer = open(fifo_path, "wb")
            self.reader = open(fifo_path, "rb")
        self.tokens_in_use = []
        self.pending_futures = []

        self.read_transport: asyncio.ReadTransport | None = None
        self.read_protocol = None

        self.started = None

    def new_token(self, token):
        # Keep a token and reuse it. Ignore cancelled Futures.
        if self.pending_futures:
            future = self.pending_futures.pop(0)
            while future.cancelled() and self.pending_futures:
                future = self.pending_futures.pop(0)
            if not future.cancelled():
                future.set_result(token)
                return
        self.read_transport.pause_reading()
        self.writer.write(token)
        self.writer.flush()

    async def __aenter__(self):
        loop = asyncio.get_event_loop()
        if self.started is None:
            self.started = asyncio.Event()
            self.read_transport, self.read_protocol = await loop.connect_read_pipe(
                lambda: _TokenProtocol(self), self.reader
            )
            self.started.set()
        await self.started.wait()
        future = loop.create_future()
        self.pending_futures.append(future)
        self.read_transport.resume_reading()
        self.tokens_in_use.append(await future)

    async def __aexit__(self, exc_type, exc, tb):
        token = self.tokens_in_use.pop()
        self.new_token(token)


def _create_semaphore():
    match = re.search(r"fifo:([^\s]+)", os.environ.get("MAKEFLAGS", ""))
    fifo_path = None
    if match:
        fifo_path = match.group(1)
        return _MakeJobClient(fifo_path=fifo_path)
    return asyncio.BoundedSemaphore(1)


shared_semaphore = _create_semaphore()
tracks = []
max_track = 0


async def run_command(command, working_directory, description=None, check_hash=[], extradeps=[]):
    """
    Runs a command asynchronously. The command should ideally be a list of strings
    and pathlib.Path objects. If all of the paths haven't been modified since the last
    time the command was run, then it'll be skipped. (The last time a command was run
    is stored based on the hash of the command.)

    The command is run from the working_directory and the paths are made relative to it.

    Description is used for logging only. If None, the command itself is logged.

    Paths in check_hash are hashed before and after the command. If the hash is
    the same, then the old mtimes are reset. This is helpful if a command may produce
    the same result and you don't want the rest of the build impacted.
    """
    paths = []
    if isinstance(command, list):
        for i, part in enumerate(command):
            if isinstance(part, pathlib.Path):
                paths.append(part)
                part = part.relative_to(working_directory, walk_up=True)
            # if isinstance(part, list):

            command[i] = str(part)
        command = " ".join(command)

    command_hash = hashlib.sha3_256(command.encode("utf-8"))
    command_hash.update(str(working_directory).encode("utf-8"))
    command_hash = command_hash.hexdigest()

    # If a command is run multiple times, then wait for the first one to continue. Don't run it again.
    if command_hash in ALREADY_RUN:
        logger.debug(f"Already running {command_hash} {command}")
        await ALREADY_RUN[command_hash].wait()
        return
    ALREADY_RUN[command_hash] = asyncio.Event()

    run_reason = None
    # If the path inputs are all older than the last time we ran them, then we don't have anything to do.
    if command_hash in LAST_BUILD_TIMES and all((p.exists() for p in paths)):
        last_build_time = LAST_BUILD_TIMES[command_hash]
        # Check all paths in the command because one must be modified by the command.
        newest_file = max((0 if p.is_dir() else p.stat().st_mtime_ns for p in paths))
        nothing_newer = newest_file <= last_build_time
        logger.debug(f"Last build time {last_build_time} Newest file {newest_file}")
        if nothing_newer:
            # Escape early if an extra dep is newer.
            for p in extradeps:
                if p.stat().st_mtime_ns > last_build_time:
                    run_reason = f"{p.relative_to(working_directory, walk_up=True)} is newer"
                    nothing_newer = False
                    break
        else:
            for p in paths:
                if p.stat().st_mtime_ns == newest_file:
                    run_reason = f"{p.relative_to(working_directory, walk_up=True)} is newer"
                    break
        if nothing_newer:
            logger.debug(f"Nothing newer {command[-32:]}")
            ALREADY_RUN[command_hash].set()
            return
    else:
        run_reason = "no previous build time"
        newest_file = 0

    file_hashes = {}
    for path in check_hash:
        if not path.exists():
            continue
        with path.open("rb") as f:
            digest = hashlib.file_digest(f, "sha256")
            stat = path.stat()
            mtimes = (stat.st_atime, stat.st_mtime)
            mtimes_ns = (stat.st_atime_ns, stat.st_mtime_ns)
            file_hashes[path] = (digest, mtimes, mtimes_ns)

    cancellation = None
    async with shared_semaphore:
        global max_track
        if not tracks:
            max_track += 1
            tracks.append(max_track)
        track = tracks.pop()
        start_time = time.perf_counter_ns() // 1000
        process = await asyncio.create_subprocess_shell(
            command,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
            cwd=working_directory,
        )

        try:
            stdout, stderr = await process.communicate()
        except asyncio.CancelledError as e:
            cancellation = e
            stdout, stderr = await process.communicate()
        end_time = time.perf_counter_ns() // 1000
        trace_entries.append(
            {
                "name": command if not description else description,
                "ph": "X",
                "pid": 0,
                "tid": track,
                "ts": start_time,
                "dur": end_time - start_time,
            }
        )
        tracks.append(track)

    if process.returncode == 0:
        old_newest_file = newest_file
        newest_file = max((p.stat().st_mtime_ns for p in paths))
        LAST_BUILD_TIMES[command_hash] = newest_file
        ALREADY_RUN[command_hash].set()

        for path in check_hash:
            if path not in file_hashes:
                continue
            with path.open("rb") as f:
                digest = hashlib.file_digest(f, "sha256")
                old_digest, _, old_mtimes_ns = file_hashes[path]
                if old_digest.digest() == digest.digest():
                    logger.debug(f"{path} is unchanged")
                    os.utime(path, ns=old_mtimes_ns)

        # If something has failed and we've been canceled, hide our success so
        # the error is clear.
        if cancellation:
            raise cancellation
        if description:
            logger.info(f"{description} ({run_reason})")
            logger.debug(command)
        else:
            logger.info(f"{command} ({run_reason})")
        if old_newest_file == newest_file:
            logger.error("No files were modified by the command.")
            raise RuntimeError()
    else:
        if command_hash in LAST_BUILD_TIMES:
            del LAST_BUILD_TIMES[command_hash]
        if stdout:
            logger.info(stdout.decode("utf-8").strip())
        if stderr:
            logger.warning(stderr.decode("utf-8").strip())
        if not stdout and not stderr:
            logger.warning("No output")
        logger.error(command)
        if cancellation:
            raise cancellation
        raise RuntimeError()


async def run_function(
    function,
    positional,
    named,
    description=None,
):
    async with shared_semaphore:
        global max_track
        if not tracks:
            max_track += 1
            tracks.append(max_track)
        track = tracks.pop()
        start_time = time.perf_counter_ns() // 1000
        result = await asyncio.to_thread(function, *positional, **named)

        end_time = time.perf_counter_ns() // 1000
        trace_entries.append(
            {
                "name": str(function) if not description else description,
                "ph": "X",
                "pid": 0,
                "tid": track,
                "ts": start_time,
                "dur": end_time - start_time,
            }
        )
        tracks.append(track)

    if description:
        logger.info(description)
        logger.debug(function)
    else:
        logger.info(function)
    return result


def run_in_thread(function):
    def wrapper(*positional, **named):
        return run_function(function, positional, named)

    return wrapper


cwd = pathlib.Path.cwd()


def parse_depfile(f):
    depfile_contents = f.read_text().split()
    extradeps = []
    for dep in depfile_contents:
        if dep == "\\" or dep[-1] == ":":
            continue
        if dep.startswith("/"):
            extradeps.append(pathlib.Path(dep))
        else:
            raise RuntimeError(f"Unexpected depfile entry {dep}")


class Compiler:
    def __init__(self, srcdir: pathlib.Path, builddir: pathlib.Path, cmake_args):
        self.c_compiler = cmake_args["CC"]
        self.ar = cmake_args["AR"]
        self.cflags = cmake_args.get("CFLAGS", "")

        self.srcdir = srcdir
        self.builddir = builddir

    async def preprocess(
        self, source_file: pathlib.Path, output_file: pathlib.Path, flags: list[pathlib.Path]
    ):
        output_file.parent.mkdir(parents=True, exist_ok=True)
        depfile = output_file.parent / (output_file.name + ".d")
        if depfile.exists():
            pass
        await run_command(
            [
                self.c_compiler,
                "-E",
                "-MMD",
                "-MF",
                depfile,
                "-c",
                source_file,
                self.cflags,
                *flags,
                "-o",
                output_file,
            ],
            description=f"Preprocess {source_file.relative_to(self.srcdir)} -> {output_file.relative_to(self.builddir)}",
            working_directory=self.srcdir,
            check_hash=[output_file],
        )

    async def compile(
        self, source_file: pathlib.Path, output_file: pathlib.Path, flags: list[pathlib.Path] = []
    ):
        if isinstance(source_file, str):
            source_file = self.srcdir / source_file
        if isinstance(output_file, str):
            output_file = self.builddir / output_file
        output_file.parent.mkdir(parents=True, exist_ok=True)
        depfile = output_file.with_suffix(".d")
        extradeps = []
        if depfile.exists():
            depfile_contents = depfile.read_text().split()
            for dep in depfile_contents:
                if dep == "\\" or dep[-1] == ":":
                    continue
                if dep.startswith("/"):
                    extradeps.append(pathlib.Path(dep))
                else:
                    extradeps.append(self.srcdir / dep)
        await run_command(
            [self.c_compiler, self.cflags, "-MMD", "-c", source_file, *flags, "-o", output_file],
            description=f"Compile {source_file.relative_to(self.srcdir)} -> {output_file.relative_to(self.builddir)}",
            working_directory=self.srcdir,
            extradeps=extradeps,
        )

    async def archive(self, objects: list[pathlib.Path], output_file: pathlib.Path):
        output_file.parent.mkdir(parents=True, exist_ok=True)
        # Do one file at a time so that we don't have a long command line. run_command
        # should skip unchanged files ok.
        input_files = output_file.with_suffix(output_file.suffix + ".input_files")
        input_file_content = "\n".join(str(p) for p in objects)
        # Windows paths have \ as separator but ar wants them as / (like UNIX)
        input_file_content = input_file_content.replace("\\", "/")
        input_files.write_text(input_file_content)
        await run_command(
            [self.ar, "rvs", output_file, f"@{input_files}"],
            description=f"Create archive {output_file.relative_to(self.srcdir)}",
            working_directory=self.srcdir,
            extradeps=objects,
        )
