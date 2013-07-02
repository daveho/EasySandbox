# EasySandbox

An easy way to sandbox untrusted C programs on Linux.
Allows them only to allocate memory (up to a fixed amount),
read and write via pre-opened file descriptors (such as stdin
and stdout), and exit.  Sandboxing is done using
[SECCOMP](http://lwn.net/Articles/332974/).

The intended use is being able to safely execute student
code submissions for the [CloudCoder](http://cloudcoder.org)
programming exercise system.

You can run

```bash
make runtests
```

to run the test programs.  If you see "All tests passed!", then
EasySandbox is working on your system.

# Using EasySandbox

Run the `make` command to build the EasySandbox shared library.

Run the program you want to sandbox, using the **LD_PRELOAD** environment
variable to load the EasySandbox shared library before the untrusted executable
is executed:

```bash
LD_PRELOAD=/path/to/EasySandbox.so ./untrustedExe
```

EasySandbox defines its own implementation of `malloc` and `free`, to ensure
that the program will not need to call `sbrk` or `mmap` to allocate memory
while in SECCOMP mode.  The heap is a fixed size, and cannot grow while the
program is running.  You can control the size of the heap by setting
the `EASYSANDBOX_HEAPSIZE` environment variable to the size of the heap
in bytes.  The default heap size is 8MB.

**Note**: EasySandbox uses [__libc_start_main](http://refspecs.linuxbase.org/LSB_3.1.1/LSB-Core-generic/LSB-Core-generic/baselib---libc-start-main-.html)
to hook into the startup process.  If the untrusted executable defines its own entry
point (rather than the normal Linux/glibc one), it could execute untrusted code.
In my intended application (compiling and executing student code
submissions), I control the compilation process, so I can take
steps to ensure that `__libc_start_main` is called.

# Limitations

When you execute a program using EasySandbox, it will print the message

```text
<<entering SECCOMP mode>>
```

followed by a newline character
to both stdout and stderr.  The reason is that the first call to print
to an output stream causes glibc to invoke `fstat`, which is not permitted
when in SECCOMP mode.  So, the EasySandbox shared library must print some output
to stdout and stderr before entering SECCOMP mode in order for these streams
to be usable.  It is fairly easy to filter out
this output as a post-processing step.

Similarly, reading from stdin also triggers a call to `fstat`.
The EasySandbox shared library works around this by putting the stdin
file descriptor into nonblocking mode, attempting to read a single
character using the `fgetc` function, and then using `ungetc` function
to put the character back if one was read.  This should not cause any
problems for programs that use C library functions to read from stdin,
but programs that use the `read` system call to read from the stdin
file descriptor may not be able to read the first byte of input.

The EasySandbox shared library implements its own `exit` function,
because glibc's invokes the `exit_group` system call, which is not allowed
by SECCOMP.  `atexit` is currently not supported.

EasySandbox is not intended to be used for multithreaded programs.
