CFLAGS = -g -Wall -fPIC

SRCS = EasySandbox.c Malloc.c memmgr.c
OBJS = $(SRCS:.c=.o)

all : $(OBJS)

clean :
	rm -f *.o
