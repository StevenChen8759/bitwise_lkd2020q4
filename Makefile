all: bitcpy

bitcpy: bitcpy.c
	gcc -g bitcpy.c -o bitcpy

clean:
	rm -rf bitcpy
