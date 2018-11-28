#include "rhj.h"

int getnextodd(int num)
{
    if(num%2==0) return num+1;
    else return num+2;
}
int computemask(int n){
  int k=1;
  for(int i=1;i<n;i++){
    k=2*k+1;
  }
  return k;
}

int H1(int a,int mask){
  a = a & mask; /* 7= 111 diladi pairnw 3 bits gia arxi */
  return a;
}


Relation* fill_table(int type)
{

  Relation *in_relation = (Relation *)malloc(sizeof(Relation));
  in_relation->num_tuples=SIZE;
  in_relation->tuples = malloc(sizeof(tuple)*in_relation->num_tuples);
  if (type==0)//random
  {
    srand(time(NULL));
    for (int i=0;i<in_relation->num_tuples;i++)
    {
      in_relation->tuples[i].key = i;
      in_relation->tuples[i].payload=rand()%100;
    }
  }
  else if (type==1)//monoi
  {
    for (int i=0,j=1;i<in_relation->num_tuples;i++,j=j+2)
    {
      in_relation->tuples[i].key = i;
      in_relation->tuples[i].payload=j;
    }

  }
  else if (type==2)//zigoi
  {
    for (int i=0,j=0;i<in_relation->num_tuples;i++,j=j+2)
    {
      in_relation->tuples[i].key = i;
      in_relation->tuples[i].payload=j;
    }

  }
  else if (type==3)//asoi
  {
    for (int i=0;i<in_relation->num_tuples;i++)
    {
      in_relation->tuples[i].key = i;
      in_relation->tuples[i].payload=1;
    }
  }
  else if (type==4)//proto asoi, ta ipoloipa miden
  {
    for (int i=0;i<in_relation->num_tuples;i++)
    {
      in_relation->tuples[i].key = i;
      if (i==0) in_relation->tuples[i].payload=1;
      else in_relation->tuples[i].payload=0;
    }
  }
  return in_relation;
}

void print_relation (Relation* in_relation)
{
  for (int i=0;i<in_relation->num_tuples;i++)
  {
    printf("%d | %d \n",in_relation->tuples[i].key,in_relation->tuples[i].payload);
  }
}


int H2(int a) //Thomas Wang integer hash method
{
  a = (a ^ 61) ^ (a >> 16);
  a = a + (a << 3);
  a = a ^ (a >> 4);
  a = a * 0x27d4eb2d;
  a = a ^ (a >> 15);
  return a;
}
