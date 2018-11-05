#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

struct tuple{
  int32_t key;    //rowID
  int32_t payload;   //value
};
typedef struct tuple tuple;

struct Relation{
  tuple  *tuples;   //pinakas apo tuples
  uint32_t num_tuples;  //megethos pinaka
};

Relation* fill_table(Relation * in_relation)
{
  srand(time(NULL));
  for (int i=0;i<in_relation->num_tuples;i++)
  {
    in_relation->tuple->key = i;
    in_relation->tuple->payload=rand()%100;
  }
  return in_relation;
}
void print_relation (Relation* in_relation)
{
  for (int i=0;i<in_relation->num_tuples;i++)
  {
    printf("%d | %d \n",in_relation->tuple->key,in_relation->tuple->payload);
  }
}

int main (void)//used for testing our testing functions
{
  Relation * A;
  A = (Relation*) malloc (sizeof(Relation));
  A->num_tuples = 100;
  A->tuples = (tuple*) malloc (sizeof(tuple))*A->num_tuples;
  A = fill_table(A);
  print_relation(A);
}
