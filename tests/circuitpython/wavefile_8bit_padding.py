# The last buffer of a WaveFile is padded to a multiple of 4 bytes; for 8-bit
# samples the pad is 0x80 (silence) and never more than 3 bytes.
try:
    import os, audiocore

    os.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMFS:
    SEC_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.SEC_SIZE)

    def readblocks(self, n, buf):
        for i in range(len(buf)):
            buf[i] = self.data[n * self.SEC_SIZE + i]
        return 0

    def writeblocks(self, n, buf):
        for i in range(len(buf)):
            self.data[n * self.SEC_SIZE + i] = buf[i]
        return 0

    def ioctl(self, op, arg):
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE


def le(value, size):
    return value.to_bytes(size, "little")


def wav_u8_mono(data):
    # PCM, mono, 16 kHz, 8 bits per sample
    fmt = le(1, 2) + le(1, 2) + le(16000, 4) + le(16000, 4) + le(1, 2) + le(8, 2)
    return (
        b"RIFF"
        + le(4 + 8 + len(fmt) + 8 + len(data), 4)
        + b"WAVEfmt "
        + le(len(fmt), 4)
        + fmt
        + b"data"
        + le(len(data), 4)
        + data
    )


bdev = RAMFS(50)
os.VfsFat.mkfs(bdev)
fs = os.VfsFat(bdev)

for length in range(4, 9):
    name = "u8-%d.wav" % length
    with fs.open(name, "wb") as f:
        f.write(wav_u8_mono(bytes(range(32, 32 + length))))
    with fs.open(name, "rb") as f:
        sample = audiocore.WaveFile(f)
        audiocore.reset_buffer(sample)  # what an AudioOut does on play()
        result, buf = audiocore.get_buffer(sample)
    print(length, result, len(buf), list(buf))

# A caller-supplied buffer is split into halves that are a multiple of 4 bytes,
# so padding the last buffer stays inside its half.
with fs.open("u8-5.wav", "rb") as f:
    sample = audiocore.WaveFile(f, bytearray(10))
    audiocore.reset_buffer(sample)
    result = 1
    while result == 1:
        result, buf = audiocore.get_buffer(sample)
        print(result, len(buf), list(buf))
