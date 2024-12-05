import audiomp3, audiocore
import ulab.numpy as np

TEST_FILE = __file__.rsplit("/", 1)[0] + "/../circuitpython-manual/audiocore/jeplayer-splash-44100-stereo.mp3"

def normalized_rms_ulab(values):
    values = np.frombuffer(values, dtype=np.int16)
    # this function works with ndarrays only
    minbuf = np.mean(values)
    values = values - minbuf
    samples_sum = np.sum(values * values)
    return (samples_sum / len(values))**.5

def print_frame_loudness(decoder, n):
    for i in range(n):
        result, buf = audiocore.get_buffer(decoder)
        print(f"{i} {result} {normalized_rms_ulab(buf):5.0f}")
    print()

# First frames
decoder = audiomp3.MP3Decoder(TEST_FILE)
print_frame_loudness(decoder, 8)

# First frames (fresh decoder)
decoder = audiomp3.MP3Decoder(TEST_FILE)
print_frame_loudness(decoder, 2)

# First frames (reopen)
decoder.open(TEST_FILE)
print_frame_loudness(decoder, 3)
