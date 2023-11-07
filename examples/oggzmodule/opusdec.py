import opus

music = open("ehren-paper_lights-96.opus", "rb")

op = opus.opus(music)
buffer = memoryview(bytearray(23040))
output = open("decoded.raw", "w+b")

min_samples = 10000000
max_samples = 0

while samples := op.read_stereo(buffer):
    min_samples = min(samples, min_samples)
    max_samples = max(samples, max_samples)
    output.write(buffer[:samples*4])

print(f"{min_samples}, {max_samples} minimum and maximum samples decoded per call")