OBJS = instruction_metrics.o instruction_names.o

all : ${OBJS}

%.o : %.c
	gcc -O3 -c -static -I ../include -o $@ -c $(realpath $<)

clean:
	rm *.o
