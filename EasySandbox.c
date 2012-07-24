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

#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include "memmgr.h"

////////////////////////////////////////////////////////////////////////
// Definitions/prototypes
////////////////////////////////////////////////////////////////////////

// Error exit codes
#define DLOPEN_ERROR      16  // dlopen() failed
#define MMAP_FAILED_ERROR 17  // couldn't allocate memory pool
#define NO_MAIN_ERROR     18  // no main() function found
#define SECCOMP_ERROR     19  // couldn't enter SECCOMP mode

// Heap size is hard-coded as 1M.
#define DEFAULT_HEAP_SIZE  (1024*1024)

// Pool memory allocation functions: will be used for
// dynamic allocation, overriding the built-in glibc memory allocator
// (which might make system calls not allowed by SECCOMP.)
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

////////////////////////////////////////////////////////////////////////
// main() function
////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	const char *shlib_filename = argv[1];
	argc--;
	argv++;

	// Load the shared library containing the untrusted code.
	// NOTE WELL: if the shared lib has an _init function
	// (i.e., constructors) it will be executed with full privileges!
	// Make sure you use objcopy to rename _init to
	// _easysandbox_init before attempting to eecute the
	// untrusted code!
	void *handle = dlopen(shlib_filename, RTLD_NOW);
	if (handle == 0) {
		_exit(DLOPEN_ERROR);
	}

	// Get pointers to _easysandbox_init and main functions.
	void (*shlib_init)(void);
	int (*shlib_main)(int argc, char **argv);

	*(void **)(&shlib_init) = dlsym(handle, "_easysandbox_init");
	*(void **)(&shlib_main) = dlsym(handle, "main");

	if (shlib_main == 0) {
		_exit(NO_MAIN_ERROR);
	}

	// At this point, it should be safe to turn on SECCOMP.
	// Enable SECCOMP mode.
	if (prctl(PR_SET_SECCOMP, 1, 0, 0) == -1) {
		_exit(SECCOMP_ERROR);
	}

	// Execute the init and main functions!
	if (shlib_init != 0) {
		shlib_init();
	}

	int rc = shlib_main(argc, argv);

	// Interestingly enough, neither exit() nor _exit() is allowed
	// by SECCOMP because they both call exit_group(), not on the
	// allowed list of system calls.  So, invoke the exit system call
	// directly.

	//_exit(rc);
	syscall(SYS_exit, rc);

	// not reached: just making gcc happy
	return 0;
}


////////////////////////////////////////////////////////////////////////
// Implementation functions
////////////////////////////////////////////////////////////////////////

// Constructor function to initialize our heap.
__attribute__((constructor))
static void malloc_init(void)
{
	unsigned long heapsize = DEFAULT_HEAP_SIZE;

	// Use mmap to create the heap.
	void *heap = mmap(0, (size_t)heapsize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	if (heap == MAP_FAILED) {
		// Couldn't allocate heap memory.
		_exit(MMAP_FAILED_ERROR);
	}

	// Initialize the heap.
	memmgr_init(heap, heapsize);
}

void *malloc(size_t size)
{
	return memmgr_alloc((ulong) size);
}

void free(void *ptr)
{
	memmgr_free(ptr);
}

void *calloc(size_t nmemb, size_t size)
{
	unsigned char *buf = malloc(nmemb * size);
	if (buf != 0) {
		unsigned char *p;
		for (p = buf; p < buf + (nmemb * size); p++) {
			*p = (unsigned char) '\0';
		}
	}
	return buf;
}

void *realloc(void *ptr, size_t size)
{
	void *buf;
	unsigned char *dst;
	unsigned char *src;
	size_t alloc_size, to_copy, i;

	// Allocate new buffer
	buf = malloc(size);

	if (buf != 0) {
		// Find original allocation size
		alloc_size = (size_t) memmgr_get_block_size(ptr);
		to_copy = alloc_size;
		if (to_copy > size) {
			to_copy = size;
		}

		// Copy data to new buffer
		dst = buf;
		src = ptr;
		for (i = 0; i < to_copy; i++) {
			*dst++ = *src++;
		}

		// Free the old buffer
		free(ptr);
	}

	return buf;
}
