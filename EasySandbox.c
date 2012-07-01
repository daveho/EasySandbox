// EasySandbox - simple sandboxing for untrusted Linux binaries using seccmop
// Copyright (c) 2012, David H. Hovemeyer <david.hovemeyer@gmail.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include "memmgr.h"

// Default heap size is 1M.  That should be plenty for programming
// exercises.
#define DEFAULT_HEAP_SIZE  (1024*1024)

// See: http://justanothergeek.chdir.org/2010/03/seccomp-as-sandboxing-solution.html

// A saved pointer to the real main() function.
// It will be called by our wrapper main() function
// (which turns on SECCOMP mode).
static int (*real_main) (int, char **, char **);

// Redefining __libc_start_main gives us a convenient way to
// hook into the start of execution (before the executable's
// constructor functions or main are executed.)
int __libc_start_main(
	int (*main) (int, char **, char **),
	int argc,
	char *__unbounded *__unbounded ubp_av,
	void (*init) (void),
	void (*fini) (void),
	void (*rtld_fini) (void),
	void *__unbounded stack_end) __attribute__ ((noreturn))
{
	// Pointer to the real __libc_start_main function in glibc.
	int (*real_libc_start_main)(
		int (*main) (int, char **, char **),
		int argc,
		char *__unbounded *__unbounded ubp_av,
		void (*init) (void),
		void (*fini) (void),
		void (*rtld_fini) (void),
		void *__unbounded stack_end);
}

#if 0
void __attribute__ ((constructor)) easysandbox_init(void);

void easysandbox_init(void)
{
#if 0
#ifdef DEBUG_INIT
	{
		int pause(void);
		pause();
	}
#endif

	// If EASYSANDBOX_HEAPSIZE environment variable is set,
	// create a heap of that size.  Otherwise, use the default
	// heap size.
	unsigned long heapsize = DEFAULT_HEAP_SIZE;
	char *heapsize_env = getenv("EASYSANDBOX_HEAPSIZE");
	if (heapsize_env != 0) {
		int converted = sscanf(heapsize_env, "%lu", &heapsize);
		if (converted != 1) {
			heapsize = DEFAULT_HEAP_SIZE;
		}
	}

	// Use mmap to create the heap.
	void *heap = mmap(0, (size_t)heapsize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS, -1, (off_t)0);
	if (heap == MAP_FAILED) {
		// Couldn't allocate heap memory.
		exit(1);
	}

	// Initialize the heap.
	memmgr_init(heap, heapsize);

	// Now we can enter SECCOMP mode.
	if (prctl(PR_SET_SECCOMP, 1, 0, 0) == -1) {
		exit(1);
	}
#endif
}
#endif
