CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11

all: s21_grep

grep.o: s21_grep.c
	$(CC) $(FLAGS) -c s21_grep.c -o s21_grep.o

s21_grep: grep.o
	$(CC) $(FLAGS) *.o  -o s21_grep

clang:
	clang-format -i --style=Google *.c *.h

rebuild:
	make clean
	make s21_grep

clean:
	rm -rf *.o s21_grep

tests:
	make s21_grep
	./tests.sh
	make clean