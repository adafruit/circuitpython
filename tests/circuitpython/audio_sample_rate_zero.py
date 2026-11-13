# Every audio source must refuse a sample rate below 1. Audio outputs and
# synthio divide by the rate, so 0 would give nonsense timing or a crash.
import array
import io
import struct

import audiocore
import synthio


def wav(rate):
    fmt = struct.pack("<HHIIHH", 1, 1, rate, rate * 2, 2, 16)
    data = b"\0\0" * 8
    body = b"WAVEfmt " + struct.pack("<I", len(fmt)) + fmt
    body += b"data" + struct.pack("<I", len(data)) + data
    return io.BytesIO(b"RIFF" + struct.pack("<I", len(body)) + body)


def midi():
    track = b"\x00\x90\x3c\x40\x60\x80\x3c\x40\x00\xff\x2f\x00"
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, 96)
    return io.BytesIO(header + b"MTrk" + struct.pack(">I", len(track)) + track)


def check(name, make, rates=(8000, 0, -1)):
    for rate in rates:
        try:
            print(name, rate, make(rate).sample_rate)
        except ValueError as e:
            print(name, rate, "ValueError:", e)


samples = array.array("h", [0] * 16)
check("RawSample", lambda rate: audiocore.RawSample(samples, sample_rate=rate))
check("MidiTrack", lambda rate: synthio.MidiTrack(b"\x00\xff\x2f\x00", tempo=96, sample_rate=rate))
check("from_file", lambda rate: synthio.from_file(midi(), sample_rate=rate))
# A WAV header holds an unsigned rate, so only 0 is out of range.
check("WaveFile", lambda rate: audiocore.WaveFile(wav(rate)), (8000, 0))
