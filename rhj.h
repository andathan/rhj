#include <math.h>
#include <stdio.h>
#include <stdint.h>

#define N 3
#define BUFFERSIZE 1024*1024/64;

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
 struct ResultNode *next;
};

struct Result{
  ResultNode *first;
  ResultNode *current;
  int32_t size;
}

typedef struct Relation Relation;
typedef struct Result Result;
typedef struct ResultNode ResultNode;

/*functions for list of results*/
void InsertResult(Result *head,int key1,int key2);
Result *ListInit();

/*radix hash join of 2 relations*/
Result* RadixHashJoin(relation *relR, relation *relS);

/*hashfunction for indexing*/
int H2(int a);

/*returns the next prime of a*/
int getnextprime(int num);
