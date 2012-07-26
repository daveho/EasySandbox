An easy way to sandbox untrusted C programs on Linux.
Allows them only to allocate memory (up to a fixed amount),
read and write via pre-opened file descriptors (such as stdin
and stdout), and exit.  Sandboxing is done using
[seccomp](http://lwn.net/Articles/332974/), specifically
the "classic" seccomp mode as opposed to the newer
[seccomp-filter](http://lwn.net/Articles/494252/) mode.

The intended use is being able to safely execute student
code submissions for the [CloudCoder](http://cloudcoder.org)
programming exercise system.

Because glibc makes LOTS of system calls forbidden by
seccomp in the course of doing simple things like printing
text to stdout or even just exiting,
EasySandbox uses [klibc](http://git.kernel.org/?p=libs/klibc/klibc.git;a=summary)
rather than glibc.  Have a look at
the Makefile to see how the test programs are compiled.
The idea is to use objcopy to rename the untrusted
program's main() function to "realmain", which is then
called by a wrapper main() function provided by EasySandbox
which enables SECCOMP.

If you are using Debian or Ubuntu, run

```bash
sudo apt-get install libklibc-dev
```

to install klibc.  Make sure you use the klcc wrapper for
gcc when you compile code to be linked against klibc.
