try:
    import microcontroller
except ImportError:
    print("SKIP")
    raise SystemExit

size = len(microcontroller.nvm)
microcontroller.nvm[0:] = bytearray([0] * size)
arr0 = microcontroller.nvm[0:]
arr1 = []
for i in range(0, size):
    microcontroller.nvm[i] = i % 255
    arr1.append(i % 255)

if arr0 != bytearray([0] * size):
    print("FAIL")

if microcontroller.nvm[0:] != bytearray(arr1):
    print("FAIL")
print("OK")
del size, arr0, arr1
