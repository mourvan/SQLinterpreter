all:
	gcc -c Table.c
	g++ -g lex.cpp syn.cpp Table.o main.cpp -o main -Wall -pedantic -fsanitize=address -Wextra -std=c++11
