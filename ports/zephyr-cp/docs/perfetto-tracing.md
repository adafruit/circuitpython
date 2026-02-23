# Perfetto Tracing

The Zephyr port supports Perfetto tracing for performance analysis. This document
describes how to capture, validate, and view traces.

## Capturing Traces

Traces are written to `circuitpython.perfetto-trace` in the port directory when
running with tracing enabled (e.g., on native_sim).

## Validating Traces

### Using trace_processor

The Perfetto trace_processor tool can validate and query trace files:

```bash
~/repos/perfetto/tools/trace_processor circuitpython.perfetto-trace
```

This will download the trace_processor binary if needed and open an interactive
SQL shell. If the trace loads successfully, you can query it:

```sql
SELECT COUNT(*) FROM slice;
```

### Using the Perfetto UI

Open https://ui.perfetto.dev and drag your trace file onto the page.

## Debugging Invalid Traces

### Common Error: Packets Skipped Due to Invalid Incremental State

If trace_processor reports packets being skipped with messages like:

```
packet_skipped_seq_needs_incremental_state_invalid
```

This means packets have `SEQ_NEEDS_INCREMENTAL_STATE` (value 2) set but no
prior packet set `SEQ_INCREMENTAL_STATE_CLEARED` (value 1) to initialize the
incremental state.

**Root Cause**: The process descriptor packet (which sets `SEQ_INCREMENTAL_STATE_CLEARED`)
must be emitted before any other trace packets.

**Diagnosis**: Use protoc to inspect the raw trace:

```bash
protoc --decode_raw < circuitpython.perfetto-trace | head -100
```

Look for field 13 (sequence_flags) in the first few packets:

- `13: 1` = SEQ_INCREMENTAL_STATE_CLEARED (good - should be first)
- `13: 2` = SEQ_NEEDS_INCREMENTAL_STATE (requires prior cleared packet)

A valid trace should have the process descriptor with `13: 1` as one of the
first packets.

**Fix**: Ensure `perfetto_start()` is called before any trace events are emitted.
The descriptor emit functions in `perfetto_encoder.c` should check:

```c
if (!started) {
    perfetto_start();
}
```

### Analyzing Raw Trace Structure

To understand the trace structure:

```bash
# Count total packets
protoc --decode_raw < circuitpython.perfetto-trace | grep -c "^1 {"

# Find all sequence_flags values
protoc --decode_raw < circuitpython.perfetto-trace | grep "13:" | sort | uniq -c

# Look for track descriptors (field 60)
protoc --decode_raw < circuitpython.perfetto-trace | grep -A20 "60 {"

# Look for process descriptors (field 3 inside track_descriptor)
protoc --decode_raw < circuitpython.perfetto-trace | grep -B5 "3 {"
```

### Key Protobuf Field Numbers

TracePacket fields:

| Field | Description |
|-------|-------------|
| 8 | timestamp |
| 10 | trusted_packet_sequence_id |
| 11 | track_event |
| 12 | interned_data |
| 13 | sequence_flags |
| 60 | track_descriptor |

TrackDescriptor fields (inside field 60):

| Field | Description |
|-------|-------------|
| 1 | uuid |
| 2 | name |
| 3 | process (ProcessDescriptor) |
| 4 | thread (ThreadDescriptor) |
| 5 | parent_uuid |

## Build Verification

After modifying tracing code, verify the build is updated:

```bash
# Check source vs object file timestamps
ls -la zephyr/subsys/tracing/perfetto/perfetto_encoder.c
ls -la zephyr/build/zephyr/subsys/tracing/perfetto/CMakeFiles/subsys__tracing__perfetto.dir/perfetto_encoder.c.obj
```

The object file timestamp must be newer than the source file timestamp. If not,
rebuild the project before capturing a new trace.

## Architecture

The tracing implementation consists of:

- `perfetto_encoder.c`: Encodes trace packets using nanopb
- `perfetto_top.c`: Implements Zephyr tracing hooks (sys_trace_*)
- `perfetto_encoder.h`: Public API and UUID definitions

Key UUIDs:

| Constant | Value | Description |
|----------|-------|-------------|
| PROCESS_UUID | 1 | Root process track |
| ISR_TRACK_UUID | 2 | Interrupt service routine track |
| TRACE_TRACK_UUID | 3 | Top-level trace track |

### Initialization Flow

1. `SYS_INIT` calls `perfetto_init()` at POST_KERNEL priority 0
2. `perfetto_init()` calls `perfetto_encoder_init()`
3. `perfetto_initialized` is set to true
4. Thread hooks start firing
5. First emit function calls `perfetto_start()`
6. `perfetto_start()` emits process descriptor with `SEQ_INCREMENTAL_STATE_CLEARED`
7. Subsequent packets use `SEQ_NEEDS_INCREMENTAL_STATE`
