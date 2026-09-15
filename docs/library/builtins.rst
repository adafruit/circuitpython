:mod:`builtins` -- builtin functions and exceptions
===================================================

..  module:: builtins
    :synopsis: builtin Python functions

All builtin functions and exceptions are described here. They are also
available via the ``builtins`` module.

For more information about built-ins, see the following CPython documentation:

* `Builtin CPython Functions <https://docs.python.org/3/library/functions.html>`_
* `Builtin CPython Exceptions <https://docs.python.org/3/library/exceptions.html>`_
* `Builtin CPython Constants <https://docs.python.org/3/library/constants.html>`_

.. note:: Not all of these functions, types, exceptions, and constants are turned
    on in all CircuitPython ports, for space reasons.

Functions and types
-------------------

.. function:: abs()

.. function:: all()

.. function:: any()

.. function:: bin()

.. class:: bool()

.. class:: bytearray()

    |see_cpython| `python:bytearray`.

.. class:: bytes()

    |see_cpython| `python:bytes`.

    .. method:: bytes.decode(encoding='utf-8', errors='strict')

        Decode the bytes object to a string using the specified *encoding*.

        MicroPython supports the following encodings:

        - ``'utf-8'`` or ``'utf8'`` - UTF-8 encoding (default)
        - ``'ascii'`` - ASCII encoding (subset of UTF-8)

        The *errors* parameter controls how decoding errors are handled:

        - ``'strict'`` - Raise a ``UnicodeError`` on invalid UTF-8 (default)
        - ``'ignore'`` - Skip invalid bytes (requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS``)
        - ``'replace'`` - Replace invalid bytes with U+FFFD '�' (requires ``MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS``)

        .. note::
            Error handler support depends on build configuration. On constrained
            systems, only ``'strict'`` mode may be available.

        Example::

            >>> b'\xc2\xa9 2024'.decode('utf-8')  # © symbol
            '© 2024'
            >>> b'hello\xffworld'.decode('utf-8', 'ignore')  # Skip invalid bytes
            'helloworld'

        Raises ``LookupError`` if the encoding is not supported, or
        ``UnicodeError`` if the data contains invalid UTF-8 and ``errors='strict'``.

.. function:: callable()

.. function:: chr()

.. function:: classmethod()

.. function:: compile()

.. class:: complex()

.. function:: delattr(obj, name)

   The argument *name* should be a string, and this function deletes the named
   attribute from the object given by *obj*.

.. class:: dict()

.. function:: dir()

.. function:: divmod()

.. function:: enumerate()

.. function:: eval()

.. function:: exec()

.. function:: filter()

.. class:: float()

.. class:: frozenset()

`frozenset()` is not enabled on the smallest CircuitPython boards for space reasons.

.. function:: getattr()

.. function:: globals()

.. function:: hasattr()

.. function:: hash()

.. function:: hex()

.. function:: id()

.. function:: input()

.. class:: int()

   .. classmethod:: from_bytes(bytes, byteorder="big", *, signed=False)

   .. method:: to_bytes(length=1, byteorder="big", *, signed=False)

.. function:: isinstance()

.. function:: issubclass()

.. function:: iter()

.. function:: len()

.. class:: list()

.. function:: locals()

.. function:: map()

.. function:: max()

.. class:: memoryview()

.. function:: min()

.. function:: next()

.. class:: object()

.. function:: oct()

.. function:: open()

.. function:: ord()

.. function:: pow()

.. function:: print()

.. function:: property()

.. function:: range()

.. function:: repr()

.. function:: reversed()

`reversed()` is not enabled on the smallest CircuitPython boards for space reasons.

.. function:: round()

.. class:: set()

.. function:: setattr()

.. class:: slice()

   The *slice* builtin is the type that slice objects have.

.. function:: sorted()

.. function:: staticmethod()

.. class:: str()

    .. method:: str.encode(encoding='utf-8')

        Encode the string to bytes using the specified *encoding*.

        MicroPython supports the following encodings:

        - ``'utf-8'`` or ``'utf8'`` - UTF-8 encoding (default)
        - ``'ascii'`` - ASCII encoding (subset of UTF-8)

        Example::

            >>> '© 2024'.encode('utf-8')  # Copyright symbol
            b'\xc2\xa9 2024'

        Raises ``LookupError`` if the encoding is not supported.

    .. method:: str.center(width)

        Return a centered string of length *width*. Padding is done using spaces.

        When Unicode support is enabled (``MICROPY_PY_BUILTINS_STR_UNICODE``), this
        method counts Unicode characters rather than bytes, ensuring proper alignment
        for multi-byte UTF-8 characters.

        Example::

            >>> 'café'.center(10)  # é is 2 bytes in UTF-8
            '   café   '

.. function:: sum()

.. function:: super()

.. class:: tuple()

.. function:: type()

.. function:: zip()


Exceptions
----------

.. exception:: ArithmeticError

.. exception:: AssertionError

.. exception:: AttributeError

.. exception:: BaseException

.. exception:: BrokenPipeError

.. exception:: ConnectionError

.. exception:: EOFError

.. exception:: Exception

.. exception:: ImportError

.. exception:: IndentationError

.. exception:: IndexError

.. exception:: KeyboardInterrupt

.. exception:: KeyError

.. exception:: LookupError

.. exception:: MemoryError

.. exception:: NameError

.. exception:: NotImplementedError

.. exception:: OSError

.. exception:: OverflowError

.. exception:: RuntimeError

.. exception:: ReloadException

   `ReloadException` is used internally to deal with soft restarts.

   Not a part of the CPython standard library

.. exception:: StopAsyncIteration

.. exception:: StopIteration

.. exception:: SyntaxError

.. exception:: SystemExit

    |see_cpython| `python:SystemExit`.

.. exception:: TimeoutError

.. exception:: TypeError

    |see_cpython| `python:TypeError`.

.. exception:: UnicodeError

.. exception:: ValueError

.. exception:: ZeroDivisionError

Constants
---------

.. data:: Ellipsis

.. data:: NotImplemented
