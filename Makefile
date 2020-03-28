all: bitcpy vector

bitcpy: bitcpy.c
	gcc -g bitcpy.c -o bitcpy

mck_bitcpy: bitcpy
	@echo "Result will write into file bitcpy_op, you can view the file later..."
	valgrind -q --leak-check=full ./bitcpy > bitcpy_op
	cat bitcpy_op | less

vector: vector.c
	gcc -g vector.c -o vector

mck_vector: vector
	valgrind -q --leak-check=full ./vector

clean:
	rm -rf bitcpy bitcpy_op vector
