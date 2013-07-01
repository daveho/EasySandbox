// Allocate some memory - should work

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	int **ptrs;
	int i, sum;

	// This code is just a fancy way of computing the sum of
	// the integers 1..100
	ptrs = malloc(sizeof(int*) * 1000); 
	for (i = 0; i < 1000; i++) {
		ptrs[i] = malloc(sizeof(int));
		*(ptrs[i]) = (i+1);
	}
	for (sum = 0, i = 0; i < 1000; i++) {
		sum += *(ptrs[i]);
	}

/*
	// Free all of the memory
	for (i = 0; i < 1000; i++) {
		free(ptrs[i]);
	}
	free(ptrs);
*/

	printf("%i\n", sum);

	return 0;
}
