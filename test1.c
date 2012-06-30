// First test program - read/write using stdin/stdout
// Should work

#include <stdio.h>

int main(void)
{
	int age;

	printf("How old are you? ");
	scanf("%d", &age);

	printf("In dog years, you're %d years old\n", age * 7);

	return 0;
}
