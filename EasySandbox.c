/*
 * EasySandbox: an extremely simple sandbox for untrusted C/C++ programs
 * Copyright (c) 2012,2013 David Hovemeyer <david.hovemeyer@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Resources:
 *
 * - http://justanothergeek.chdir.org//2010/03/seccomp-as-sandboxing-solution/
 *
 *   This is where the idea (and code) to use __libc_start_main as a hook
 *   into the startup process came from.  However, my implementation is
 *   slightly different, in that I enable SECCOMP before any of the
 *   constructor functions run. (Without this modification, constructor
 *   functions would run with full privileges.)
 *
 * - http://www.win.tue.nl/~aeb/linux/lk/lk-14.html
 *
 *   Very practical advice on using SECCOMP.
 */

#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

#define DLOPEN_FAILED  120
#define SECCOMP_FAILED 121
#define EXIT_FAILED    122  /* should not happen */

/* Saved pointers to the real init and main functions. */
static void (*real_init)(void);
static int (*real_main)(int, char **, char **);

/*
 * Statically-allocated region of memory with which to
 * implement a custom sbrk() routine.  This is used by
 * the memory allocator in malloc.c to implement
 * malloc/free and friends.  This approach allows us
 * to support malloc/free without any system calls.
 *
 * FIXME: make this dynamically allocated (using mmap to allocate memory
 * before entering SECCOMP mode)
 */
static char s_heap[EASYSANDBOX_HEAPSIZE];
static char *s_brk = &s_heap[0];
static unsigned s_alloc_count;

/*
 * Custom implementation of sbrk() that allocates from a fixed-size
 * array of bytes.  This avoids the need for malloc/free and
 * friends to make any system calls.
 */
void *sbrk(intptr_t incr)
{
	intptr_t used, remaining;
	void *newbrk;

	used = s_brk - s_heap;
	remaining = EASYSANDBOX_HEAPSIZE - used;
	
	if (remaining < incr) {
		/*fprintf(stderr, "sbrk: failed to allocate %ld after %u allocations\n", incr, s_alloc_count);*/
		errno = ENOMEM;
		return (void*) -1;
	}
	/*fprintf(stderr, "sbrk: allocated %ld\n", (long) incr);*/
	newbrk = s_brk;
	s_brk += incr;
	s_alloc_count++;
	return newbrk;
}

static void wrapper_init(void)
{
	/* The first call to print to a stream will cause glibc to
	 * invoke the fstat system call, which will cause SECCOMP
	 * to kill the process. There does not seem to be any way
	 * of working around this problem except to print some output
	 * on the stdout and strerr streams before entering SECCOMP mode.
	 * Unfortunately, a printf call that generates no output doesn't
	 * work, so some extraneous output seems unavoidable. Fortunately,
	 * this is easy to filter out as a post-processing step. */
	fprintf(stdout, "<<entering SECCOMP mode>>\n");
	fflush(stdout);
	fprintf(stderr, "<<entering SECCOMP mode>>\n");
	fflush(stderr);

#if 1
	/* Enter SECCOMP mode */
	if (prctl(PR_SET_SECCOMP, 1, 0, 0) == -1) {
		_exit(SECCOMP_FAILED);
	}
#endif

	/* Call the real init function */
	real_init();
}

static int wrapper_main(int argc, char **argv, char **envp)
{
	/* Call the real main function.  Note that we can't
	 * actually return, because glibc will attempt to call the
	 * exit_group function, which will cause SECCOMP to kill
	 * the process.  So, directly invoke the exit system
	 * call. Also, flush stdout and stderr. */
	int n;
	n = real_main(argc, argv, envp);
	fflush(stdout);
	fflush(stderr);
	syscall(SYS_exit, n);
	return EXIT_FAILED;
}

int __libc_start_main(
	int (*main)(int, char **, char **),
	int argc,
	char ** ubp_av,
	void (*init)(void),
	void (*fini)(void),
	void (*rtld_fini)(void),
	void (* stack_end))
{
	void *libc_handle;

	int (*real_libc_start_main)(
		int (*main) (int, char **, char **),
		int argc,
		char ** ubp_av,
		void (*init)(void),
		void (*fini)(void),
		void (*rtld_fini)(void),
		void (* stack_end));

	real_init = init;
	real_main = main;

	/* explicitly open the glibc shared library */
	libc_handle = dlopen("libc.so.6", RTLD_LOCAL | RTLD_LAZY);
	if (libc_handle == 0) {
		_exit(DLOPEN_FAILED);
	}

	/* get a pointer to the real __libc_start_main function */
	*(void **) (&real_libc_start_main) = dlsym(libc_handle, "__libc_start_main");

	/* Delegate to the real __libc_start_main, but provide our
	 * wrapper init and main functions */
	return real_libc_start_main(wrapper_main, argc, ubp_av, wrapper_init, fini, rtld_fini, stack_end);
}
