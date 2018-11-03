#include "rhj.h"

int main (void)
{
  int num_of_buckets = pow(2,N);
  Relation *A,*B , *A_ , *B_;
  Relation * chosen_table;
  Relation * other_table;
  int *hashtable;
  int *chain;
  int *chosen_offsets;
  int *other_offsets;
  int *histogram_A;
  int *histogram_B;
  int *offsets_A;
  int *offsets_B;
  int chosen_histogram,other_histogram;
  int i,j,k,hash_key,range_hashfunction,previous,new,payload1,payload2,key1,key2,rowID;


  Result *results = ListInit();

  A = fill_table(0);
  B = fill_table(0);
  histogram_A=(int*)malloc(sizeof(int)*A->num_tuples);
  for(i=0;i<A->num_tuples;i++)
  {
      histogram_A[i]=0;
  }

  histogram_B=(int*)malloc(sizeof(int)*B->num_tuples);
  for(i=0;i<B->num_tuples;i++)
  {
      histogram_B[i]=0;
  }

  histogram_A = make_histogram(A);
  offsets_A = make_offsets(histogram_A);
  A_=segmentation(A,histogram_A,offsets_A);

  histogram_B = make_histogram(B);
  offsets_B = make_offsets(histogram_B);
  B_=segmentation(B,histogram_B,offsets_B);

  free(offsets_A);
  free(offsets_B);
  offsets_A = make_offsets(histogram_A);
  offsets_B = make_offsets(histogram_B);



  for(i=0;i<num_of_buckets;i++) printf("%d | ",histogram_B[i] );
  printf("\n");
  for(i=0;i<num_of_buckets;i++) printf("%d | ",histogram_A[i] );
  printf("\n");
  for (i=0;i<num_of_buckets;i++) //gia ola ta buckets
  {
    if (histogram_A[i]<histogram_B[i]) //brisko to bucket me ligotera stoixeia, kai pernaw ta stoixeia tou se pointers. pernaw kai ta stoixeia tou allou bucket
    //**se deutero stadio mporoume na kaloume apla sinartisi me katallila orismata, oste na min xriazontai oi metablites chosen kai other.**
    {
      printf("I will choose A with %d\n",histogram_A[i] );
      chosen_histogram=histogram_A[i];
      other_histogram=histogram_B[i];
      chosen_table = A_;
      other_table = B_;
      chosen_offsets=offsets_A;
      other_offsets=offsets_B;
    }
    else
    {
      printf("I will choose B with %d\n",histogram_B[i] );
      chosen_histogram = histogram_B[i];
      other_histogram = histogram_A[i];
      chosen_table = B_;
      other_table= A_;
      chosen_offsets=offsets_B;
      other_offsets=offsets_A;
    }
    /*range of hashfunction*/
    printf("%d\n",chosen_histogram );
    range_hashfunction=getnextprime(chosen_histogram);
    if(range_hashfunction==0) continue;
    /*alloc chain array and initialize it with -1*/
    chain = (int *)malloc(sizeof(int)*chosen_histogram);
    for(k=0;k<chosen_histogram;k++)
      chain[k] = -1;

    /**hashtable pinakas aka bucket*/
    hashtable = (int *)malloc(sizeof(int)*range_hashfunction);
    for (k=0;k<range_hashfunction;k++)//kai ton arxikopoio me -1
      hashtable[k]=-1;

    /*for every tuple in bucket*/
    for (j=0;j<chosen_histogram;j++)
    {
      printf("ektelestike\n");
      hash_key = H2(chosen_table->tuples[chosen_offsets[i]+j].payload) % range_hashfunction;
      previous = hashtable[hash_key];
      printf("Chosen offsets is %d and j is %d",chosen_offsets[i],j);
      new = chosen_table->tuples[chosen_offsets[i]+j].key; //pairnw to kainourgio klidi
      hashtable[hash_key] = new; //kai to bazw sto hashtable
      /*an ipirxe idi allo klidi me idio hash paw ston pinaka chain, stin thesi new kai bazw to progioumeno
      an den ipirxe den kanw tipota afou idi o pinakas chain einai arxikopoihmenos se -1*/
      if (previous!=-1)
        chain[new] = previous;
    }

    for(k=0;k<range_hashfunction;k++){
      printf("h:%d\n",hashtable[k] );
      printf("value = %d \n",hashtable[k] );
    }//kai ton arxikopoio me -1
     //gia to allo bucket

    for (j=0;j<other_histogram;j++)//gia ola ta tuples tou allou bucket
    {
      hash_key= H2(other_table->tuples[other_offsets[i]+j].payload) % range_hashfunction;//omoia me to chosen table
      printf("hash = %d\n",hash_key );
      rowID = hashtable[hash_key]; //pairnw to rowID apo to hashtable
      printf("rowid = %d\n",rowID );
      while (rowID!=-1)//oso auto einai -1, diladi oso iparxoun kai alles eggrafes me idio hash
      {
        payload1 = chosen_table->tuples[chosen_offsets[i]+rowID].payload;
        payload2 = other_table->tuples[other_offsets[i]+j].payload;
        printf("%d == %d\n",payload1,payload2 );
        if (payload1==payload2)//kanw ton elenxo
        {
          key1=chosen_table->tuples[chosen_offsets[i]+rowID].payload;
          key2=other_table->tuples[other_offsets[i]+j].payload;
          InsertResult(results,key1,key2);
        }
        rowID = chain[rowID];//pairnw tin proigoumeni eggrafi meso tou chain
      }
    }
  }
  printf("print results\n");
  PrintResults(results);
}
