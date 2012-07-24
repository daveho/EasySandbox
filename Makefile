EXE_NAME = EasySandbox

CFLAGS = -g -Wall -D_GNU_SOURCE -fPIC $(DEBUG)

SRCS = EasySandbox.c memmgr.c
OBJS = $(SRCS:.c=.o)

TEST_SRCS = test1.c
TEST_EXES = $(TEST_SRCS:.c=.so)

all : $(EXE_NAME) $(TEST_EXES)

$(EXE_NAME) : $(OBJS)
	gcc -o $@ $(OBJS) -ldl

%.so : %.o
	gcc -shared -o $@ $*.o
	objcopy --redefine-sym _init=_easysandbox_init $@ $@

clean :
	rm -f *.o *.so
