# CIRCUITPY-CHANGE: micropython does not have this test file
# subscripting a subclassed tuple
class Foo(tuple):
    pass

foo = Foo((1,2))
foo[0]
