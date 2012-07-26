CC = klcc
CFLAGS = -g -Wall

TEST_EXES = test1 test2 test3 test4 test5

all : EasySandbox.o $(TEST_EXES)

%_hooked.o : %.o
	objcopy --redefine-sym main=realmain $*.o $*_hooked.o

test% : test%_hooked.o EasySandbox.o
	klcc -o $@ $@_hooked.o EasySandbox.o

test%_nosandbox : test%.o
	klcc -o $@ test$*.o

runtests :
	./runalltests.sh $(TEST_EXES)

clean :
	rm -f *.o $(TEST_EXES)
