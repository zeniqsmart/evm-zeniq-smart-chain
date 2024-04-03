OBJS = keccak.o keccakf800.o

all : ${OBJS}

%.o : %.c
	gcc -O3 -c -static -I ../include -o $@ -c $(realpath $<)

clean:
	rm *.o
