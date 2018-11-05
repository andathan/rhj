#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "lists.h"

#define N 3
#define SIZE 1000

struct tuple{
  int32_t key;    //rowID
  int32_t payload;   //value
};
typedef struct tuple tuple;

struct Relation{
  tuple  *tuples;   //pinakas apo tuples
  uint32_t num_tuples;  //megethos pinaka
};


/*body of functions in general_functions.c*/

int H2(int a);          /*hashfunction for indexing*/
int computemask(int n); /*returns the integer formed from N times 1 on binary*/
int getnextodd(int num); /*returns the next odd number of a*/
void print_relation (Relation* in_relation);
Relation* fill_table();
/*hash function for segmentation*/
int H1(int a,int mask); /*returns given a & mask */

/*body of functions in rhj.c*/

int *make_histogram(Relation *in_relation); /*returns the histogram table of given relation*/
int *make_offsets(int *histogram); /*returns the Psum of given relation*/
Relation * segmentation(Relation *,int *,int *);
Result *IndexAndResult(int ,int ,int ,int ,Relation *,Relation *,Result *);
Result *RadixHashJoin(Relation *, Relation *);
