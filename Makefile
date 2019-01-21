all: rhj

rhj:   main.c inbetween.c rhj.c datastore.c predicates.c thpool.c trees.c
	gcc -g3 -o radixhash main.c inbetween.c rhj.c datastore.c thpool.c predicates.c trees.c -lm -lpthread
	
