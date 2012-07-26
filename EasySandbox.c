// EasySandbox - sandboxing for untrusted code using Linux/seccomp
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
#include <sys/prctl.h>

#define DEFAULT_HEAP_SIZE (1024*1024)

#define SECCOMP_ERROR 17

// "realmain" is the main function of the untrusted program.
// Even though it is probably defined as "main" in the source code,
// we'll use objcopy to rename it to ensure that our
// main() function is called first.
extern int realmain(int argc, char **argv, char **envp);

// Wrapper for main
int main(int argc, char **argv, char **envp)
{
	// FIXME: make this configurable
	size_t heapsize = DEFAULT_HEAP_SIZE;

	const size_t FUDGE = 4096;

	// Attempt to create a large malloc arena that has a substantial
	// amount of memory available in it, enough that the memory allocations
	// that the process will need to do can be satisfied without the
	// need to allocate a new arena (which would call sbrk or mmap,
	// thus killing the process since it will be in SECCOMP mode.)
	// There's no robust way to do this.  Our current approach is to
	// do a large allocation, use realloc to make it smaller (hoping that
	// this will create a small amount of available space at the end of
	// the arena), do a small allocation (hoping it will be placed in the
	// freed space at the end of the arena), and then freeing the large
	// chunk.  If all goes well this will leave a large chunk of
	// available space at the beginning of the arena which can be used
	// to satisfy future allocations.

	char *p1, *p1_realloc, *p2;

	// Allocate a generous chunk of memory
	p1 = malloc(heapsize + FUDGE);

	// Make it smaller
	p1_realloc = realloc(p1, heapsize);
	if (p1_realloc != p1) {
		fprintf(stderr, "Warning: realloc moved the large allocation\n");
	}

	// Allocate a very small chunk
	p2 = malloc(1);

	if (p2 < (p1 + heapsize) || p2 > (p1 + heapsize + FUDGE)) {
		fprintf(stderr,
			"Warning: small allocation not in same arena as large allocation? (p1=%p, p2=%p)\n",
			p1, p2);
	}

	// Free the large chunk
	free(p1);

	// Now we can enter SECCOMP mode.
	if (prctl(PR_SET_SECCOMP, 1, 0, 0) < 0) {
		_exit(SECCOMP_ERROR);
	}

	return realmain(argc, argv, envp);
}
