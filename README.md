An easy way to sandbox untrusted C programs on Linux.
Allows them only to allocate memory (up to a fixed amount),
read and write via pre-opened file descriptors (such as stdin
and stdout), and exit.  Sandboxing is done using
[seccomp](http://lwn.net/Articles/332974/), specifically
the [seccomp-filter](http://lwn.net/Articles/494252/) mode,
which may or may not be supported by your Linux distribution.
Ubuntu 12.04 and variants do support it.

The intended use is being able to safely execute student
code submissions for the [CloudCoder](http://cloudcoder.org)
programming exercise system.

EasySandbox uses [dietlibc](http://www.fefe.de/dietlibc/)
rather than glibc.  The main reason is that glibc uses system
calls in undesirable ways: for example, calling `printf` can
result in a call to `mmap`, and we probably don't want untrusted
code calling `mmap`.  In theory we might add filter rules to
allow "acceptable" calls to `mmap`, but for now it's easier to
just use a simpler libc implementation (dietlibc) that doesn't make these
calls.

If you are using Debian or Ubuntu, run

```bash
sudo apt-get install dietlibc-dev
```

to install dietlibc.  Make sure you use the `diet gcc` wrapper for
gcc when you compile code to be linked against dietlibc.

Have a look at the Makefile to see how the sandboxed test programs are compiled.
You can run

```bash
make runtests
```

to run the test programs.  If you see "All tests passed!", then
seccomp is working on your system.

Note that in the test programs, the program startup is hijacked
by renaming the untrusted program's `main` function and calling
it from a wrapper `main` function that enables seccomp.
Note that if you are using a libc that supports constructor functions,
this will not be effective since they run before main.
Dietlibc does not support constructor functions.
