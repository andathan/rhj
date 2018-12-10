all: rhj

rhj:   main.c inbetween.c rhj.c datastore.c predicates.c lists.c
	gcc -g3 -o rhj main.c inbetween.c rhj.c datastore.c predicates.c lists.c -lm


