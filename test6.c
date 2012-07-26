// Make sure that constructor functions are NOT executed.
// klibc doesn't support them, and we assume therefore
// that main() is the true start of the untrusted code.
// If a constructor function IS executed, then we are executing
// untrusted code with full privileges!

#include <stdio.h>

__attribute((constructor))
void myinit(void)
{
	printf("If you see this, you lose!\n");
}

int main(void)
{
	// Just return without producing any output
	return 0;
}
