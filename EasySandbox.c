#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include "memmgr.h"

// Default heap size is 1M.  That should be plenty for programming
// exercises.
#define DEFAULT_HEAP_SIZE  (1024*1024)

void __attribute__ ((constructor)) easysandbox_init(void);

void easysandbox_init(void)
{
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
}
