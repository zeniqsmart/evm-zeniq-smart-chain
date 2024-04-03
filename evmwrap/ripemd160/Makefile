OBJS = memzero.o ripemd160.o

all : ${OBJS}

%.o : %.c
	gcc -O3 -static -c -o $@ -c $(realpath $<)

clean:
	rm *.o
