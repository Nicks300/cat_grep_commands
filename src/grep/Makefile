CC=gcc
CFLAGS=-Wall -Werror -Wextra

all: s21_grep

s21_grep.o: s21_grep.c
	$(CC) $(FLAGS) -c s21_grep.c -o s21_grep.o

s21_grep: s21_grep.o
	$(CC) $(CFLAGS) *.o -o s21_grep

clang:
	clang-format -i --style=Google *.c *.h

clean:
	rm -rf *.o s21_grep
	rm -rf *.log

rebuild: clean s21_grep

tests:
	make s21_grep
	./tests.sh
	make clean