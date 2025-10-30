import os
import subprocess


def get_version_info_from_git(repo_path, extra_args=[]):
    if "CP_VERSION" in os.environ:
        git_tag = os.environ["CP_VERSION"]
    else:
        # Note: git describe doesn't work if no tag is available
        try:
            git_tag = subprocess.check_output(
                # CIRCUITPY-CHANGE
                [
                    "git",
                    "describe",
                    "--dirty",
                    "--tags",
                    "--always",
                    "--match",
                    "[1-9].*",
                    *extra_args,
                ],
                cwd=repo_path,
                stderr=subprocess.STDOUT,
                universal_newlines=True,
            ).strip()
        # CIRCUITPY-CHANGE
        except subprocess.CalledProcessError as er:
            if er.returncode == 128:
                # git exit code of 128 means no repository found
                return None
            git_tag = ""
        except OSError:
            return None
    try:
        # CIRCUITPY-CHANGE
        git_hash = subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            cwd=repo_path,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
        ).strip()
    except subprocess.CalledProcessError:
        # CIRCUITPY-CHANGE
        git_hash = "unknown"
    except OSError:
        return None

    # CIRCUITPY-CHANGE
    try:
        # Check if there are any modified files.
        subprocess.check_call(
            ["git", "diff", "--no-ext-diff", "--quiet", "--exit-code"],
            cwd=repo_path,
            stderr=subprocess.STDOUT,
        )
        # Check if there are any staged files.
        subprocess.check_call(
            ["git", "diff-index", "--cached", "--quiet", "HEAD", "--"],
            cwd=repo_path,
            stderr=subprocess.STDOUT,
        )
    except subprocess.CalledProcessError:
        git_hash += "-dirty"
    except OSError:
        return None

    # CIRCUITPY-CHANGE
    # Try to extract MicroPython version from git tag
    ver = git_tag.split("-")[0].split(".")

    return git_tag, git_hash, ver


if __name__ == "__main__":
    import pathlib
    import sys

    git_tag, _, _ = get_version_info_from_git(pathlib.Path("."), sys.argv[1:])
    if git_tag is None:
        sys.exit(-1)
    print(git_tag)
