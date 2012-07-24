An easy way to sandbox untrusted processes on Linux.
Allows them only to allocate memory (up to a fixed amount),
read and write via pre-opened file descriptors (such as stdin
and stdout), and exit.  Uses seccomp.

The intended use is being able to safely execute student
code submissions for the [CloudCoder](http://cloudcoder.org)
programming exercise system.

This code is at a REALLY REALLY early stage, and many improvements
are needed (for example, calling `exit()` or `_exit()` kills the
process, destructor functions are not executed).  However, the basic
technique does work, and I'm somewhat confident the remaining issues
can be dealt with.

TODO: documentation.

If you want to see how it works, have a look at runtest.sh -
it shows how an untrusted executable is run.
