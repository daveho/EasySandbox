CC = gcc
CXX = g++
COMMON_FLAGS = -g -Wall -D_BSD_SOURCE 
CFLAGS = -std=c99 $(COMMON_FLAGS) #-DDEBUG_MALLOC
CXXFLAGS = $(COMMON_FLAGS)
SHLIB_CFLAGS = -fPIC $(CFLAGS)

TEST_SRCS := $(shell ls t/test*.c*)
TEST_EXES := $(patsubst %.c,%,$(patsubst %.cpp,%,$(TEST_SRCS)))

t/test% : t/test%.c
	$(CC) $(CFLAGS) -o $@ t/test$*.c -lm

t/test% : t/test%.cpp
	$(CXX) $(CXXFLAGS) -o $@ t/test$*.cpp -lm

all : EasySandbox.so tests

EasySandbox.so : EasySandbox.o malloc.o
	$(CC) -shared -o EasySandbox.so EasySandbox.o malloc.o -ldl

EasySandbox.o : EasySandbox.c
	$(CC) -c $(SHLIB_CFLAGS) EasySandbox.c

malloc.o : malloc.c
	$(CC) -c $(SHLIB_CFLAGS) malloc.c

tests : $(TEST_EXES)

runtests : all
	./runalltests.sh $(TEST_EXES)

clean :
	rm -f *.o *.so $(TEST_EXES) core
