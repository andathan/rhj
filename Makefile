all: rhj

rhj:  main.c general_functions.c lists.c rhj.c
	gcc -g3 -o rhj main.c general_functions.c lists.c rhj.c -lm


