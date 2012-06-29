An easy way to sandbox untrusted processes on Linux.
Allows them only to allocate memory (up to a fixed amount),
read and write via pre-opened file descriptors (such as stdin
and stdout), and exit.

Uses seccomp.
