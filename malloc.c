/*
 * Copyright (C) 2012 Bryan Cuccioli
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
 * Original source for this code:
 * https://github.com/bcuccioli/malloc
 */

/* High-level overview: This works by maintaining a singly-linked list
 * of previously-used memory segments that have been freed. */

#define NALLOC 1024

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

/* This header is stored at the beginning of memory segments in the list. */
union header {
  struct {
    union header *next;
    unsigned len;
  } meta;
  long x; /* Presence forces alignment of headers in memory. */
};

static union header list;
static union header *first = NULL;

void free(void* ptr) {
  if (ptr == NULL) {
    return;
  }

  union header *iter, *block;
  iter = first;
  block = (union header*)ptr - 1;

  /* Traverse to the spot in the list to insert the freed fragment,
   * such that the list is ordered by memory address (for coalescing). */
  while (block <= iter || block >= iter->meta.next) {
    if ((block > iter || block < iter->meta.next) &&
        iter >= iter->meta.next) {
      break;
    }
    iter = iter->meta.next;
  }

  /* If the new fragment is adjacent in memory to any others, merge
   * them (we only have to check the adjacent elements because the
   * order semantics are enforced). */
  if (block + block->meta.len == iter->meta.next) {
    block->meta.len += iter->meta.next->meta.len;
    block->meta.next = iter->meta.next->meta.next;
  } else {
    block->meta.next = iter->meta.next;
  }
  if (iter + iter->meta.len == block) {
    iter->meta.len += block->meta.len;
    iter->meta.next = block->meta.next;
  } else {
    iter->meta.next = block;
  }

  first = iter;
}

void *malloc(size_t size) {
  union header *p, *prev;
  prev = first;
  unsigned true_size =
    (size + sizeof(union header) - 1) /
     sizeof(union header) + 1;

  /* If the list of previously allocated fragments is empty,
   * initialize it. */
  if (first == NULL) {
    prev = &list;
    first = prev;
    list.meta.next = first;
    list.meta.len = 0;
  }
  p = prev->meta.next;
  /* Traverse the list of previously allocated fragments, searching
   * for one sufficiently large to allocate. */
  while (1) {
    if (p->meta.len >= true_size) {
      if (p->meta.len == true_size) {
        /* If the fragment is exactly the right size, we do not have
         * to split it. */
        prev->meta.next = p->meta.next;
      } else {
        /* Otherwise, split the fragment, returning the first half and
         * storing the back half as another element in the list. */
        p->meta.len -= true_size;
        p += p->meta.len;
        p->meta.len = true_size;
      }
      first = prev;
      return (void *)(p+1);
    }
    /* If we reach the beginning of the list, no satisfactory fragment
     * was found, so we have to request a new one. */
    if (p == first) {
      char *page;
      union header *block;
      /* We have to request memory of at least a certain size. */
      if (true_size < NALLOC) {
        true_size = NALLOC;
      }
      page = sbrk((intptr_t) (true_size * sizeof(union header)));
      if (page == (char *)-1) {
        /* There was no memory left to allocate. */
        errno = ENOMEM;
        return NULL;
      }
      /* Create a fragment from this new memory and add it to the list
       * so the above logic can handle breaking it if necessary. */
      block = (union header *)page;
      block->meta.len = true_size * sizeof(union header);
      free((void *)(block + 1));
      p = first;
    }

    prev = p;
    p = p->meta.next;
  }
  return NULL;
}

void* calloc(size_t num, size_t len) {
  void* ptr = malloc(num * len);

  /* Set the allocated array to 0's.*/
  if (ptr != NULL) {
    memset(ptr, 0, num * len);
  }

  return ptr;
}

void* realloc(void *ptr, size_t new_size) {
  size_t old_size = 0; /* XXX yolo */
  void* new = malloc(new_size);

  if (new == NULL) {
    /* malloc() failed, insufficient memory remaining. */
    errno = ENOMEM;
  } else {
    /* We cannot grow the memory segment. */
    if (new_size > old_size) {
      new_size = old_size;
    }
    memmove(new, ptr, new_size);
    free(ptr);
  }

  return new;
}

