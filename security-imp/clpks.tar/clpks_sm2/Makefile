CC = gcc
CFLAGS = -Wall -O2 

SRC = sm2.c sm3.c sm2alg.c
OBJ = sm2.o sm3.o sm2alg.o

LIB = libclpk.so
TEST = sm2_test

.PHONY: all

all: $(LIB) $(TEST)

.c.o:
	$(CC) $(CFLAGS) -c -fPIC $*.c -I. -I./miracl/ -o $*.o

$(LIB): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ)  -L./miracl/ -lmiracl -shared -o $(LIB)

$(TEST): sm_test.o $(LIB)
	$(CC) -o $@ sm_test.o  -L./ libclpk.so -lpthread  #-L./miracl/ -lmiracl

.PHONY: clean distclean

clean:
	rm -rf *.o *.a *.so sm2_test

distclean:
	rm -rf *.o $(LIB) $(TEST)
