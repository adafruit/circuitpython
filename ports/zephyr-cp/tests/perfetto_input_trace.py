# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries LLC
# SPDX-License-Identifier: MIT

"""Utilities for creating Perfetto input trace files for native_sim tests.

This module can be used directly from Python or from the command line:

    python -m tests.perfetto_input_trace input_trace.json output.perfetto

Input JSON format — counter tracks use [timestamp, value] pairs, instant
tracks use bare timestamps:

{
  "gpio_emul.01": [[8000000000, 0], [9000000000, 1], [10000000000, 0]],
  "display_capture": [500000000, 1000000000]
}
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Mapping, Sequence

# Counter tracks: list of (timestamp, value) pairs.
# Instant tracks: list of timestamps (bare ints).
InputTraceData = Mapping[str, Sequence[tuple[int, int] | int]]


def _load_perfetto_pb2():
    from perfetto.protos.perfetto.trace import perfetto_trace_pb2 as perfetto_pb2

    return perfetto_pb2


def _is_instant_track(events: Sequence) -> bool:
    """Return True if *events* is a list of bare timestamps (instant track)."""
    if not events:
        return False
    return isinstance(events[0], int)


def build_input_trace(trace_data: InputTraceData, *, sequence_id: int = 1):
    """Build a Perfetto Trace protobuf for input replay counter and instant tracks."""
    perfetto_pb2 = _load_perfetto_pb2()
    trace = perfetto_pb2.Trace()

    seq_incremental_state_cleared = 1
    seq_needs_incremental_state = 2

    for idx, (track_name, events) in enumerate(trace_data.items()):
        track_uuid = 1001 + idx
        instant = _is_instant_track(events)

        desc_packet = trace.packet.add()
        desc_packet.timestamp = 0
        desc_packet.trusted_packet_sequence_id = sequence_id
        if idx == 0:
            desc_packet.sequence_flags = seq_incremental_state_cleared
        desc_packet.track_descriptor.uuid = track_uuid
        desc_packet.track_descriptor.name = track_name
        if not instant:
            desc_packet.track_descriptor.counter.unit = (
                perfetto_pb2.CounterDescriptor.Unit.UNIT_COUNT
            )

        if instant:
            for ts in events:
                event_packet = trace.packet.add()
                event_packet.timestamp = ts
                event_packet.trusted_packet_sequence_id = sequence_id
                event_packet.sequence_flags = seq_needs_incremental_state
                event_packet.track_event.type = perfetto_pb2.TrackEvent.Type.TYPE_INSTANT
                event_packet.track_event.track_uuid = track_uuid
        else:
            for ts, value in events:
                event_packet = trace.packet.add()
                event_packet.timestamp = ts
                event_packet.trusted_packet_sequence_id = sequence_id
                event_packet.sequence_flags = seq_needs_incremental_state
                event_packet.track_event.type = perfetto_pb2.TrackEvent.Type.TYPE_COUNTER
                event_packet.track_event.track_uuid = track_uuid
                event_packet.track_event.counter_value = value

    return trace


def write_input_trace(
    trace_file: Path, trace_data: InputTraceData, *, sequence_id: int = 1
) -> None:
    """Write input replay data to a Perfetto trace file."""
    trace = build_input_trace(trace_data, sequence_id=sequence_id)
    trace_file.parent.mkdir(parents=True, exist_ok=True)
    trace_file.write_bytes(trace.SerializeToString())


def _parse_trace_json(data: object) -> dict[str, list[tuple[int, int]] | list[int]]:
    if not isinstance(data, dict):
        raise ValueError("top-level JSON value must be an object")

    parsed: dict[str, list[tuple[int, int]] | list[int]] = {}
    for track_name, events in data.items():
        if not isinstance(track_name, str):
            raise ValueError("track names must be strings")
        if not isinstance(events, list):
            raise ValueError(f"track {track_name!r} must map to a list of events")

        if not events:
            parsed[track_name] = []
            continue

        # Distinguish instant (bare ints) vs counter ([ts, value] pairs).
        if isinstance(events[0], (int, float)):
            parsed[track_name] = [int(ts) for ts in events]
        else:
            parsed_events: list[tuple[int, int]] = []
            for event in events:
                if not isinstance(event, (list, tuple)) or len(event) != 2:
                    raise ValueError(
                        f"track {track_name!r} events must be [timestamp, value] pairs "
                        "or bare timestamps"
                    )
                timestamp_ns, value = event
                parsed_events.append((int(timestamp_ns), int(value)))
            parsed[track_name] = parsed_events

    return parsed


def _build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Generate a Perfetto input trace file used by native_sim --input-trace"
    )
    parser.add_argument("input_json", type=Path, help="Path to input trace JSON")
    parser.add_argument("output_trace", type=Path, help="Output .perfetto file path")
    parser.add_argument(
        "--sequence-id",
        type=int,
        default=1,
        help="trusted_packet_sequence_id to use (default: 1)",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = _build_arg_parser()
    args = parser.parse_args(argv)

    trace_json = json.loads(args.input_json.read_text())
    trace_data = _parse_trace_json(trace_json)
    write_input_trace(args.output_trace, trace_data, sequence_id=args.sequence_id)

    print(f"Wrote {args.output_trace} ({len(trace_data)} tracks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
