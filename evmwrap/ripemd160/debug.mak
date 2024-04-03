OBJS = memzero.o ripemd160.o

all: ${OBJS}

%.o: %.c
	gcc -O0 -g -fPIC -c -o $@ -c $(realpath $<)

