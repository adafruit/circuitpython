# CIRCUITPY-CHANGE: micropython does not have this test file
def gen():
    yield from gen()

try:
    print(list(gen()))
except RuntimeError:
    print("RuntimeError")
