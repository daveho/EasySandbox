SHLIB_NAME = easysandbox.so

# Call pause() in easysandbox_init so debugger can attach to it
DEBUG = -DDEBUG_INIT

CFLAGS = -g -Wall -fPIC $(DEBUG)

SRCS = EasySandbox.c Malloc.c memmgr.c
OBJS = $(SRCS:.c=.o)

TEST_SRCS = test1.c
TEST_EXES = $(TEST_SRCS:.c=)

all : $(SHLIB_NAME) $(TEST_EXES)

$(SHLIB_NAME) : $(OBJS)
	gcc -shared -o $@ $(OBJS)

test1 : test1.c

clean :
	rm -f *.o *.so
