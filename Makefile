CC = gcc
CFLAGS = -g -Wall
SHLIB_CFLAGS = -fPIC $(CFLAGS) -DEASYSANDBOX_HEAPSIZE=8388608

TEST_EXES = test1 test2 test3 test4 test5 test6 test7

all : EasySandbox.so $(TEST_EXES)

EasySandbox.so : EasySandbox.o malloc.o
	gcc -shared -o EasySandbox.so EasySandbox.o malloc.o -ldl

EasySandbox.o : EasySandbox.c
	gcc -c $(SHLIB_CFLAGS) EasySandbox.c

malloc.o : malloc.c
	gcc -c $(SHLIB_CFLAGS) malloc.c

test% : test%.o
	$(CC) -o $@ test$*.o

runtests : all
	./runalltests.sh $(TEST_EXES)

clean :
	rm -f *.o *.so $(TEST_EXES) core
