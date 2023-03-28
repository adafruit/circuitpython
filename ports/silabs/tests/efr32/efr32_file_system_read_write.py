try:
    import os
    import random
except ImportError:
    print("SKIP")
    raise SystemExit

random_str_list = ""
for i in range(0, 127):
    random_str_list += chr(i)

write_str = "".join(random.choice(random_str_list) for i in range(512))

f = open("efr32_file_system_read_write.txt", "w")
f.write(write_str)
f.close()

f = open("efr32_file_system_read_write.txt")
read_str = f.read()
f.close()
os.remove("efr32_file_system_read_write.txt")

if write_str != read_str:
    print("FAIL")

print("OK")
del random_str_list, write_str
