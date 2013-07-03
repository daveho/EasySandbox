CC = gcc
CXX = g++
COMMON_FLAGS = -g -Wall -D_BSD_SOURCE 
CFLAGS = -std=c99 $(COMMON_FLAGS) #-DDEBUG_MALLOC
CXXFLAGS = $(COMMON_FLAGS)
SHLIB_CFLAGS = -fPIC $(CFLAGS)

TEST_EXES = test1 test2 test3 test4 test5 test6 test7 test8 test9

test% : test%.c
	$(CC) $(CFLAGS) -o $@ test$*.c

test% : test%.cpp
	$(CXX) $(CXXFLAGS) -o $@ test$*.cpp

all : EasySandbox.so $(TEST_EXES)

EasySandbox.so : EasySandbox.o malloc.o
	gcc -shared -o EasySandbox.so EasySandbox.o malloc.o -ldl

EasySandbox.o : EasySandbox.c
	gcc -c $(SHLIB_CFLAGS) EasySandbox.c

malloc.o : malloc.c
	gcc -c $(SHLIB_CFLAGS) malloc.c

runtests : all
	./runalltests.sh $(TEST_EXES)

clean :
	rm -f *.o *.so $(TEST_EXES) core
