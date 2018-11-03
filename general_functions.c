#include "rhj.h"

int getnextprime(int num)
{
    char flag;
    for (int i = num;; ++i)
    {
      flag=0;
      for (int j=2;j<sqrt(num);j++)//psaxno mexri tin riza tou arithmou, afou meta tin riza apoklietai na exei diairetes pou den exoume idi tsekarei
      {
        if (i%j==0)
        flag=1;//brika kapoion arithmo pou na diairetai
        break;//ara den einai Prime
      }
      if(flag==0)//den exw brei kanenan arithmo pou na diaireitai
        {
        printf("Next Prime of %d is %d.\n",num,i);//ara einai prime
        return i;
      }
    }
}

int H1(int a)
{
  a = a && 7; /* 7= 111 diladi pairnw 3 bits gia arxi */
  return a;
}

int *make_histogram(Relation *in_relation){
  int index_bucket,j;
  int *histogram =(int *)malloc(sizeof(int)*pow(2,N));

  for(j=0;j<pow(2,N);j++){
    histogram[j]=0;
  }

  for (j=0; j<in_relation->num_tuples; j++){
    index_bucket = H1(in_relation->tuples[j].payload);
    histogram[index_bucket]++;
  }
  return histogram;
}

int *make_offsets(int *histogram){
  int *offsets = (int*)malloc(sizeof(int)*pow(2,N));
  for(int i=0;i<pow(2,N);i++){
    if(i==0){
      offsets[0]= 0;
    }
    else{
      offsets[i] = histogram[i-1] + offsets[i-1];
    }
  }
  return offsets;
}

Relation* segmentation(Relation *in_relation,int *histogram,int *offsets){

  if(in_relation==NULL) return NULL;
  int index_bucket;   //index_bucket = se pio bucket tha mpei auto to tuple
  Relation* table_new=(Relation*)malloc(sizeof(Relation));
  table_new->num_tuples=in_relation->num_tuples;
  table_new->tuples =(tuple*)malloc(sizeof(tuple)*table_new->num_tuples);

  for (int j=0; j<in_relation->num_tuples; j++){
    index_bucket=H1(in_relation->tuples[j].payload);

    table_new->tuples[offsets[index_bucket]].key=in_relation->tuples[j].key; // rowID,value
    table_new->tuples[offsets[index_bucket]].payload=in_relation->tuples[j].payload;
    (offsets[index_bucket])++;
  }
  return table_new;
}

Relation* fill_table(int type)
{

  Relation *in_relation = (Relation *)malloc(sizeof(Relation));
  in_relation->num_tuples=100;
  in_relation->tuples = malloc(sizeof(tuple)*in_relation->num_tuples);
  if (type==0)//random
  {srand(time(NULL));
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
