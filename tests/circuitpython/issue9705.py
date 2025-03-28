import audiomp3, audiocore

TEST_FILE = (
    __file__.rsplit("/", 1)[0]
    + "/../circuitpython-manual/audiocore/jeplayer-splash-44100-stereo.mp3"
)


def loudness(values):
    return sum(abs(a) for a in values)


def print_frame_loudness(decoder, n):
    for i in range(n):
        result, buf = audiocore.get_buffer(decoder)
        print(f"{i} {result} {loudness(buf):5.0f}")
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
