#include "rhj.h"

int main (void)
{
  int num_of_buckets = pow(2,N);
  Relation *A,*B;
  Relation * chosen_table;
  Relation * other_table;
  int *hashtable;
  int *chain;
  int *chosen_offsets;
  int *other_offsets;
  int chosen_histogram,other_histogram;
  int i,j,k,hash_key,range_hashfunction,previous,new,payload1,payload2,key1,key2;
  Result *results = ListInit();
  
  for (i=0;i<num_of_buckets;i++) //gia ola ta buckets
  {
    if (histogram_A[i]<histogram_B[i]) //brisko to bucket me ligotera stoixeia, kai pernaw ta stoixeia tou se pointers. pernaw kai ta stoixeia tou allou bucket
    //**se deutero stadio mporoume na kaloume apla sinartisi me katallila orismata, oste na min xriazontai oi metablites chosen kai other.**
    {
      chosen_histogram=histogram_A[i];
      other_histogram=histogram_B[i];
      chosen_table = A;
      other_table = B;
      chosen_offsets=offsets_A;
      other_offsets=offsets_B;
    }
    else
    {
      chosen_histogram = histogram_B[i];
      other_histogram = histogram_A[i];
      chosen_table = B;
      other_table= A;
      chosen_offsets=offsets_B;
      other_offsets=offsets_A;
    }

    /*range of hashfunction*/
    range_hashfunction=getnextprime(chosen_histogram);

    /*alloc chain array and initialize it with -1*/
    chain = (int *)malloc(sizeof(int)*chosen_histogram);
    for (k=0;k<chosen_histogram;k++)
      chain[k] = -1;

    /**hashtable pinakas aka bucket*/
    hashtable = (int *)malloc(sizeof(int)*range_hashfunction);
    for (k=0;k<range_hashfunction;k++)//kai ton arxikopoio me -1
      hashtable[k]=-1;

    /*for every tuple in bucket*/
    for (j=0;j<chosen_histogram;j++)
    {
      hash_key = H2(chosen_table->tuples[chosen_offsets[i]+j]->payload) % range_hashfunction;
      previous = hashtable[hash_key];
      new = chosen_table->tuples[chosen_offsets[i]+j]->key; //pairnw to kainourgio klidi
      hashtable[hash_key] = new; //kai to bazw sto hashtable
      /*an ipirxe idi allo klidi me idio hash paw ston pinaka chain, stin thesi new kai bazw to progioumeno
      an den ipirxe den kanw tipota afou idi o pinakas chain einai arxikopoihmenos se -1*/
      if (previous!=-1)
        chain[new] = previous;
    }
    //gia to allo bucket
    for (j=0;j<other_histogram;j++)//gia ola ta tuples tou allou bucket
    {
      hash_key= H2(other_table->tuples[other_offsets[i]+j]->payload) % range_hashfunction;//omoia me to chosen table
      rowID = hashtable[hash_key]; //pairnw to rowID apo to hashtable
      while (rowID!=-1)//oso auto einai -1, diladi oso iparxoun kai alles eggrafes me idio hash
      {
        payload1 = chosen_table->tuples[chosen_offsets[i]+rowID]->payload;
        payload2 = other_table->tuples[other_offsets[i]+j]->payload;
        if (payload1==payload2)//kanw ton elenxo
        {
          key1=chosen_table->tuples[chosen_offsets[i]+rowID]->key;
          key2=other_table->tuples[other_offsets[i]+j]->key;
          InsertResult(results,key1,key2);
        }
        rowID = chain[rowID];//pairnw tin proigoumeni eggrafi meso tou chain
      }
    }
  }
}
