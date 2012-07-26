CC = klcc
CFLAGS = -g -Wall

TEST_EXES = test1 test2 test3 test4 test5 test6

all : EasySandbox.o $(TEST_EXES)

%_hooked.o : %.o
	objcopy --redefine-sym main=realmain $*.o $*_hooked.o

test% : test%_hooked.o EasySandbox.o
	$(CC) -o $@ $@_hooked.o EasySandbox.o

test%_nosandbox : test%.o
	$(CC) -o $@ test$*.o

runtests :
	./runalltests.sh $(TEST_EXES)

clean :
	rm -f *.o $(TEST_EXES)
