#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define N 3
#define BUFFERSIZE 10//1024*1024/64
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

struct ResultNode{
 int32_t buffer[BUFFERSIZE][2];
 int32_t size;
 struct ResultNode *next;
};

typedef struct ResultNode ResultNode ;

struct Result{
  ResultNode *first;
  ResultNode *current;
};

typedef struct Relation Relation;
typedef struct Result Result;
typedef struct ResultNode ResultNode;

/*functions for list of results*/
void InsertResult(Result *head,int rowID1,int rowID2);
Result *ListInit();
void print_relation (Relation* in_relation);
void PrintResults(Result *head);
/*radix hash join of 2 relations*/
Result* RadixHashJoin(Relation *relR, Relation *relS);

/*hashfunction for indexing*/
int H2(int a);
int computemask(int n);
/*returns the next odd number of a*/
int getnextodd(int num);
/*hash function for segmentation*/
int H1(int a,int mask);
/*returns the offsets table, the histogram table and the new tables as segments*/
Relation * segmentation(Relation *,int *,int *);

Relation* fill_table();
int *make_histogram(Relation *in_relation);
int *make_offsets(int *histogram);
void deletelist(Result *head);
