// Try an illegal system call: process should be killed

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void) {
	int fd = open("test3.c", O_RDONLY);
	printf("Uh-oh: we should not have been able to open a file\n");
	return 0;
}
