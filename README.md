An easy way to sandbox untrusted C programs on Linux.
Allows them only to allocate memory (up to a fixed amount),
read and write via pre-opened file descriptors (such as stdin
and stdout), and exit.  Sandboxing is done using
[seccomp](http://lwn.net/Articles/332974/).

The intended use is being able to safely execute student
code submissions for the [CloudCoder](http://cloudcoder.org)
programming exercise system.

You can run

```bash
make runtests
```

to run the test programs.  If you see "All tests passed!", then
seccomp is working on your system.

**Note**: This library isn't completely working yet.  In particular,
reading from stdin causes SECCOMP to kill the process.  I'm working
on this, and hope to find a solution soon.
