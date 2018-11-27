#ifndef _RHJ_H__
#define _RHJ_H__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define N 3
#define SIZE 1000

typedef struct inbet_list inbet_list;
typedef struct inbetween_results inbetween_results;

struct tuple{
  int32_t key;    //rowID
  int32_t payload;   //value
};
typedef struct tuple tuple;

struct Relation{
  tuple  *tuples;   //pinakas apo tuples
  uint32_t num_tuples;  //megethos pinaka
};
typedef struct Relation Relation;

struct relation_data{
  uint64_t numColumns;
  uint64_t numTuples;
  Relation **columns;     //pinakas apo deiktes stin arxi kathe column
};
typedef struct relation_data relation_data;

struct all_data{
  relation_data ** table;  //array of pointers to relation_data
  int num_relations;       //number of total relations
};
typedef struct all_data all_data;



/*body of functions in rhj.c*/

int H2(int a);          /*hashfunction for indexing*/
int computemask(int n); /*returns the integer formed from N times 1 on binary*/
int getnextodd(int num); /*returns the next odd number of a*/
void print_relation (Relation* in_relation);
Relation* fill_table();
/*hash function for segmentation*/
int H1(int a,int mask); /*returns given a & mask */


int *make_histogram(Relation *in_relation); /*returns the histogram table of given relation*/
int *make_offsets(int *histogram); /*returns the Psum of given relation*/
Relation * segmentation(Relation *,int *,int *);
void IndexAndResult(int ,int ,int ,int ,Relation *,Relation *,inbet_list *,inbet_list *);
void RadixHashJoin(Relation *, Relation *,inbet_list *,inbet_list *);
Relation *BuildRelation(inbet_list *list,Relation *init_relation);

/*body of functions in datastore.c*/

relation_data **find_corresponding(char * token,all_data *datatable,int *num_relations);
void compute_operation(char op, int constant,Relation * relation,inbet_list * A);
void show_results(inbetween_results *inb_results,all_data *input_data, char * token);
relation_data *parsefile(char * filename);

#endif