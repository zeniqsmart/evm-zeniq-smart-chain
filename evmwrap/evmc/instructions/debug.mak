OBJS = instruction_metrics.o instruction_names.o

all: ${OBJS}

%.o: %.c
	gcc -O0 -g -c -fPIC -I ../include -o $@ -c $<

