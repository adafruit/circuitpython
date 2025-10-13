try:
    from collections import deque
except ImportError:
    print("SKIP")
    raise SystemExit


class DequeSubclass(deque):
    def __init__(self, values, maxlen):
        super().__init__(values, maxlen)

    def pop(self):
        print("pop")
        return super().pop()

    def popleft(self):
        print("popleft")
        return super().popleft()

    def append(self, value):
        print("append")
        return super().append(value)

    def appendleft(self, value):
        print("appendleft")
        return super().appendleft(value)

    def extend(self, value):
        print("extend")
        return super().extend(value)


d = DequeSubclass([1, 2, 3], 10)
print(d.append(4))
print(d.appendleft(0))
print(d.pop())
print(d.popleft())
d.extend([6, 7])
# calling list() tests iteration.
print(list(d))
