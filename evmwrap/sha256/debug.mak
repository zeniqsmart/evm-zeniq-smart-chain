all: sha256.o

%.o: %.c
	if [ `uname -p ` = aarch64 ]; then gcc -march=armv8-a+crypto -O0 -g -fPIC -c -I. -o $@ -c $<; else gcc -msse4.1 -msha -O0 -g -fPIC -c -I. -o $@ -c $<; fi

