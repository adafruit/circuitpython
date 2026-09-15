# A note whose envelope has decayed to 0 but whose slot hasn't been reaped
# yet should re-press like a fresh note, not silently vanish.
from synthio import Synthesizer, Note, Envelope
from audiocore import get_buffer

# release_time=0 collapses the envelope to level 0 in a single step, while
# the note object still holds its channel.
quick = Envelope(release_time=0)
synth = Synthesizer()
note = Note(440, envelope=quick)

synth.press(note)
get_buffer(synth)
synth.release(note)
get_buffer(synth)
print("pressed after decay:", len(synth.pressed))

synth.press(note)
print("pressed right after re-press:", len(synth.pressed))
get_buffer(synth)
print("pressed after one more render:", len(synth.pressed))
for _ in range(3):
    print("{} {:.2f}".format(*synth.note_info(note)))
    get_buffer(synth)
