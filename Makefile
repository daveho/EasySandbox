CC = diet gcc
CFLAGS = -g -Wall

TEST_EXES = test1 test2 test3 test4 test5 test6 test7

all : EasySandbox.o $(TEST_EXES)

%_hooked.o : %.o
	objcopy --redefine-sym main=realmain $*.o $*_hooked.o

test% : test%_hooked.o EasySandbox.o
	$(CC) -o $@ $@_hooked.o EasySandbox.o

# Build a non-sandboxed version of a test program.
# Useful because you can use strace on it to see what
# system calls it is making.
test%_nosandbox : test%.o
	$(CC) -o $@ test$*.o

runtests : all
	./runalltests.sh $(TEST_EXES)

clean :
	rm -f *.o $(TEST_EXES) $(TEST_EXES:%=%_nosandbox) core
