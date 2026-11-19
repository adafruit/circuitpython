:mod:`errno` -- system error codes
===================================

.. module:: errno
   :synopsis: system error codes

|see_cpython_module| :mod:`python:errno`.

This module provides access to symbolic error codes for `OSError` exception.
Most SAMD21 boards, including all SAMD21 boards without external flash, do not include :mod:`errno`.
A few other small builds also do not include it.
To check a particular board, see the :ref:`module-support-matrix`.

The available codes depend on the build:

* Most builds provide ``EACCES``, ``EADDRINUSE``, ``EAGAIN``, ``EALREADY``, ``EBADF``,
  ``EBUSY``, ``ECONNABORTED``, ``ECONNREFUSED``, ``ECONNRESET``, ``EEXIST``,
  ``EHOSTUNREACH``, ``EINPROGRESS``, ``EINVAL``, ``EIO``, ``EISDIR``, ``ENOBUFS``,
  ``ENODEV``, ``ENOENT``, ``ENOMEM``, ``ENOSPC``, ``ENOTCONN``, ``EOPNOTSUPP``,
  ``EPERM``, ``EROFS``, and ``ETIMEDOUT``.
* SAMD21 builds that include :mod:`errno` provide only ``EACCES``, ``EAGAIN``, ``EEXIST``,
  ``EINVAL``, ``EIO``, ``EISDIR``, ``ENODEV``, ``ENOENT``, ``ENOMEM``, and ``EPERM``.

``dir(errno)`` lists the codes available on a particular board.

Constants
---------

.. data:: EEXIST, EAGAIN, etc.

    Error codes, based on ANSI C/POSIX standard. All error codes start with
    "E". Errors are usually accessible as ``exc.errno``
    where ``exc`` is an instance of `OSError`. Usage example::

        try:
            os.mkdir("my_dir")
        except OSError as exc:
            if exc.errno == errno.EEXIST:
                print("Directory already exists")

.. data:: errorcode

    Dictionary mapping numeric error codes to strings with symbolic error
    code (see above)::

        >>> print(errno.errorcode[errno.EEXIST])
        EEXIST
