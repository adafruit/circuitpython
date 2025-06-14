import os

os.umount("/")


class RAMBlockDevice:
    ERASE_BLOCK_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.ERASE_BLOCK_SIZE)

    def readblocks(self, block, buf, off=0):
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            buf[i] = self.data[addr + i]

    def writeblocks(self, block, buf, off=None):
        if off is None:
            # erase, then write
            off = 0
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            self.data[addr + i] = buf[i]

    def ioctl(self, op, arg):
        if op == 4:  # block count
            return len(self.data) // self.ERASE_BLOCK_SIZE
        if op == 5:  # block size
            return self.ERASE_BLOCK_SIZE
        if op == 6:  # erase block
            return 0


bdev = RAMBlockDevice(64)
os.VfsFat.mkfs(bdev)
os.mount(os.VfsFat(bdev), "/")

content_good = b"""
# comment
key0 = "hello world"
key2= "\\n"
key3 ="\\u00c1x"
key4 = "\\U000000c1x"
key5 = "\\f\\"\\\\"
key6 = "\\t\\r\\b"
key7 = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
key8 = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
 key1 = "hello comment" # comment
ikey0 = 7
  ikey1 = 0x7f # comment
ikey2 = 0
[section]
subvalue = "hi"
"""

content_bad = [
    b'key = "\n',
    b'key = """\n',
    b"key =\n",
    b'key="',
    b'key="\0',
    b"key = strings must be quoted\n",
    b"key=7",
    # These all return None even though some of them look a bit like they might define key...
    b"",
    b"key",
    b"key ",
    b"key\0",
    b'key\0="x"',
    b"\n",
]


def run_test(key, content, getter=os.getenv):
    with open("/settings.toml", "wb") as f:
        f.write(content)

    try:
        v = getter(key)
        print(key, repr(v))
    except Exception as e:
        print(key, str(e))


print("*** Tests that should succeed ***")
run_test("key", b"")

for i in range(10):
    run_test(f"key{i}", content_good)
for i in range(4):
    run_test(f"ikey{i}", content_good, getenv_int)

# Run the test again but on CRLF content
content_good = content_good.replace(b"\n", b"\r\n")
for i in range(10):
    run_test(f"key{i}", content_good)
for i in range(4):
    run_test(f"ikey{i}", content_good, getenv_int)

print()
print("*** Integer tests that should succeed***")
run_test(f"K", b"K = 7\r\n", getenv_int)

# Test value without trailing newline
run_test(f"noeol", b"noeol=3", getenv_int)

print("Tests that should fail")
for content in content_bad:
    run_test("key", content)

# (note that getenv_int displays additional info on output)
run_test(f"K", b'K = "7"\r\n', getenv_int)
