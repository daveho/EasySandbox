// Allocate some memory - should work

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	int **ptrs;
	int i, sum;

	ptrs = malloc(sizeof(int*) * 1000); 

	for (i = 0; i < 1000; i++) {
		ptrs[i] = malloc(sizeof(int));
		*(ptrs[i]) = (i+1);
	}

	for (sum = 0, i = 0; i < 1000; i++) {
		sum += *(ptrs[i]);
	}

	// PROBLEM: glibc's printf can call fstat and mmap!
	printf("%i\n", sum);

	return 0;
}
