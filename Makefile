all: rhj

rhj:  index_build.c general_functions.c lists.c rhj.c
	gcc -g3 -o rhj index_build.c general_functions.c lists.c rhj.c -lm


