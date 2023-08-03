try:
    from collections import OrderedDict
except ImportError:
    print("SKIP")
    raise SystemExit

d = OrderedDict([(10, 20), ("b", 100), (1, 2)])
print(len(d))
print(list(d.keys()))
print(list(d.values()))
del d["b"]
print(len(d))
print(list(d.keys()))
print(list(d.values()))

# access remaining elements after deleting
print(d[10], d[1])

# add an element after deleting
d["abc"] = 123
print(len(d))
print(list(d.keys()))
print(list(d.values()))

# pop an element
print(d.popitem())
print(len(d))
print(list(d.keys()))
print(list(d.values()))

# add an element after popping
d["xyz"] = 321
print(len(d))
print(list(d.keys()))
print(list(d.values()))

# pop until empty
print(d.popitem())
print(d.popitem())
try:
    d.popitem()
except:
    print('empty')

# fromkeys returns the correct type and order
d = dict.fromkeys('abcdefghij')
print(type(d) == dict)
d = OrderedDict.fromkeys('abcdefghij')
print(type(d) == OrderedDict)
print(''.join(d))

# fromkey handles ordering with duplicates
d = OrderedDict.fromkeys('abcdefghijjihgfedcba')
print(''.join(d))

# move_to_end works as expected
d = OrderedDict()
try:
    d.move_to_end("empty")
except KeyError:
    print("empty")

d = OrderedDict.fromkeys("ab")
d.move_to_end("a")
print(''.join(d))

d.move_to_end("a", False)
print(''.join(d))

try:
    d.move_to_end("KeyError")
except KeyError:
    print("KeyError")

d.move_to_end("a", "not_a_bool")
print(''.join(d))
