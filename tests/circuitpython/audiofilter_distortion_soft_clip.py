# soft_clip is declared MP_ARG_BOOL; asking for False should give back False,
# not whatever happened to be on the stack under the argument union.
from audiofilters import Distortion

RATE = 8000


def make(soft_clip):
    return Distortion(soft_clip=soft_clip, sample_rate=RATE).soft_clip


print("default          ", Distortion(sample_rate=RATE).soft_clip)
print("soft_clip=False  ", make(False))
print("soft_clip=True   ", make(True))
