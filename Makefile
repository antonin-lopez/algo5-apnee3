CFLAGS= -g -Wall -Werror -DDEBUG_ARBRE
CC=gcc

all: huff_encode huff_decode

huff_encode : huff_encode.o arbrebin.o fap.o bfile.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

huff_decode : huff_decode.o arbrebin.o bfile.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

test_generator: test_generator.c
	$(CC) $(CFLAGS) $< -o $@

# Nouvelle règle test
.PHONY: test
test: all test_generator
	./test_generator
	./test_analysis.sh

# Dépendances obtenues avec "gcc -MM *.c"
arbrebin.o: arbrebin.c arbrebin.h
bfile.o: bfile.c bfile.h
fap.o: fap.c fap.h arbrebin.h
huff_decode.o: huff_decode.c arbrebin.h bfile.h
huff_encode.o: huff_encode.c fap.h arbrebin.h bfile.h huffman_code.h

.PHONY: clean
clean:
	rm -f huff_encode huff_decode test_generator $(patsubst %.c,%.o,$(wildcard *.c))
	rm -rf tests

.PHONY: reindent
reindent:
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4}" -i *.c *.h
