all: rhj

rhj:   main.c inbetween.c rhj.c datastore.c predicates.c trees.c
	gcc -g3 -o rhj main.c inbetween.c rhj.c datastore.c trees.c predicates.c -lm


