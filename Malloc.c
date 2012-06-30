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

// Wrappers for memory allocation functions.
// These will take precedence over the libc versions,
// and are implemented using the memmgr_* functions,
// where buffers are allocated out of an anonymous chunk of
// memory produced by mmap.

#include <stddef.h>
#include "memmgr.h"

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

