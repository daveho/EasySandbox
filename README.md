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

Because glibc makes LOTS of system calls forbidden by
seccomp in the course of doing simple things like printing
text to stdout or even just exiting,
Have a look at the Makefile to see how the test programs are compiled.
The idea is to use objcopy to rename the untrusted
program's main() function to "realmain", which is then
called by a wrapper main() function provided by EasySandbox
which enables SECCOMP.

EasySandbox uses [dietlibc](http://www.fefe.de/dietlibc/)
rather than glibc.

If you are using Debian or Ubuntu, run

```bash
sudo apt-get install dietlibc-dev
```

to install dietlibc.  Make sure you use the `diet gcc` wrapper for
gcc when you compile code to be linked against dietlibc.
