OBJS = keccak.o keccakf800.o

all: ${OBJS}

%.o: %.c
	gcc -O0 -g -c -fPIC -I ../include -o $@ -c $<

