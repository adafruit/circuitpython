# Checks the freq_shift rate updates, the no-sample read and the echo buffer limit.
import array
import math
import gc
import audiocore
import audiodelays
import synthio

sine = array.array("h", [int(32767 * math.sin(2 * math.pi * i / 256)) for i in range(256)])


def effect(delay_ms, freq_shift=True, max_delay_ms=50, decay=0.0):
    synth = synthio.Synthesizer(sample_rate=48000, waveform=sine)
    synth.press(synthio.Note(frequency=440))
    echo = audiodelays.Echo(
        max_delay_ms=max_delay_ms,
        delay_ms=delay_ms,
        decay=decay,
        mix=1.0,
        sample_rate=48000,
        freq_shift=freq_shift,
    )
    echo.play(synth)
    return echo


def render(delay_ms, blocks=100, freq_shift=True):
    echo = effect(delay_ms, freq_shift)
    return [bytes(audiocore.get_buffer(echo)[1]) for _ in range(blocks)]


# 39.99 ms is less than one 8-bit rate step from 40 ms.
print("rate fraction", render(40.0) != render(39.99))
print("repeatable", render(40.0) == render(40.0))

# These LFOs move less than one sample period in the blocks rendered.
slow = synthio.LFO(rate=0.01, offset=40, scale=1)
print("slow lfo moves rate", render(slow, 90) != render(40.0, 90))
falling = synthio.LFO(rate=0.01, offset=40, scale=-1)
print("no freq_shift holds length", render(falling, 50, False) == render(40.0, 50, False))

# With no sample, decay 1 and delay at max, the echo repeats every 480 samples.
echo = effect(10.0, max_delay_ms=10, decay=1.0)
for _ in range(10):
    audiocore.get_buffer(echo)
echo.stop()
for _ in range(10):
    audiocore.get_buffer(echo)
tail = b"".join(bytes(audiocore.get_buffer(echo)[1]) for _ in range(8))
print("no sample repeats", any(tail) and tail[:960] == tail[960:1920])
del echo, tail
gc.collect()


def limit(sample_rate, max_delay_ms, freq_shift):
    # The limit is checked before the buffer is allocated.
    try:
        audiodelays.Echo(max_delay_ms=max_delay_ms, sample_rate=sample_rate, freq_shift=freq_shift)
    except MemoryError:
        pass
    except ValueError:
        return "ValueError"
    finally:
        gc.collect()
    return "accepted"


print(131071, limit(131071, 4000, True))
print(131072, limit(131072, 4000, True))
print(131072, "no freq_shift", limit(131072, 4000, False))
