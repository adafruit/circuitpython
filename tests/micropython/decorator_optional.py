# micropython.native/viper(optional=True): native code where the build can emit it,
# bytecode where it can't


@micropython.native(optional=True)
def nat(n):
    s = 0
    for i in range(n):
        s += i
    return s


@micropython.viper(optional=True)
def vip(limit: int) -> int:
    x = 0
    while x < limit:
        x = x + 1
    return x


print(nat(100), vip(1000))
