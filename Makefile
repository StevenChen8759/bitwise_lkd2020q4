all: bitcpy

bitcpy: bitcpy.c
	gcc -g bitcpy.c -o bitcpy

mck_bitcpy: bitcpy
	@echo "Result will write into file bitcpy_op, you can view the file later..."
	valgrind -q --leak-check=full ./bitcpy > bitcpy_op
	cat bitcpy_op | less

clean:
	rm -rf bitcpy
