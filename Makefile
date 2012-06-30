SHLIB_NAME = easysandbox.so

CFLAGS = -g -Wall -fPIC

SRCS = EasySandbox.c Malloc.c memmgr.c
OBJS = $(SRCS:.c=.o)

$(SHLIB_NAME) : $(OBJS)
	gcc -shared -o $@ $(OBJS)

clean :
	rm -f *.o *.so
