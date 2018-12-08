#include "rhj.h"
#include "inbetween.h"

int *make_histogram(Relation *in_relation){
  int index_bucket,j;
  int *histogram =(int *)malloc(sizeof(int)*pow(2,N));
  int mask;
  for(j=0;j<pow(2,N);j++){
    histogram[j]=0;
  }
  mask = computemask(N);
  for (j=0; j<in_relation->num_tuples; j++){
    index_bucket = H1(in_relation->tuples[j].payload,mask);
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
  Relation* seg_relation=(Relation*)malloc(sizeof(Relation));
  seg_relation->num_tuples=in_relation->num_tuples;
  seg_relation->tuples =(tuple*)malloc(sizeof(tuple)*seg_relation->num_tuples);

  int mask = computemask(N);
  for (int j=0; j<in_relation->num_tuples; j++){
    index_bucket=H1(in_relation->tuples[j].payload,mask);

    seg_relation->tuples[offsets[index_bucket]].key=in_relation->tuples[j].key; // rowID,value
    seg_relation->tuples[offsets[index_bucket]].payload=in_relation->tuples[j].payload;
    (offsets[index_bucket])++;
  }
  return seg_relation;
}


void IndexAndResult(int index_tuples,int comp_tuples,int index_Psum,int comp_Psum,Relation *indexRel,Relation *compRel,inbet_list *res1,inbet_list *res2){

  int j,k,rowID,hash_key,new,previoys,payload1,payload2,range_hashfunction,previous;
  int *chain=NULL , *hashtable=NULL ;
  uint64_t key1,key2;
  if(index_tuples==0) return ;
  int results_c=0;
  range_hashfunction=getnextodd(index_tuples);

  chain = (int *)malloc(sizeof(int)*index_tuples);
  for(k=0;k<index_tuples;k++){
    chain[k] = -1;
  }



  hashtable = (int *)malloc(sizeof(int)*range_hashfunction);
  for (k=0;k<range_hashfunction;k++)
    hashtable[k]=-1;

  /*for every tuple in bucket*/
  for (j=0;j<index_tuples;j++){
    hash_key = H2(indexRel->tuples[index_Psum+j].payload) % range_hashfunction;
    previous = hashtable[hash_key];
    new = j; //pairnw to kainourgio klidi
    hashtable[hash_key] = new;
    if (previous!=-1)
      chain[new] = previous;
  }
  for(j=0;j<comp_tuples;j++){
    hash_key= H2(compRel->tuples[comp_Psum+j].payload) % range_hashfunction;
    rowID = hashtable[hash_key];
    while (rowID!=-1){
      payload1 = indexRel->tuples[index_Psum+rowID].payload;
      payload2 = compRel->tuples[comp_Psum+j].payload;
      if (payload1==payload2){
        key1=(int)indexRel->tuples[index_Psum+rowID].key;
        key2=(int)compRel->tuples[comp_Psum+j].key;
        InsertInbetList(res1,(int)key1);
        InsertInbetList(res2,(int)key2);
        results_c++;
      }
      rowID = chain[rowID];//pairnw tin proigoumeni eggrafi meso tou chain
    }
  }
  free(hashtable);
  free(chain);
}


void RadixHashJoin(Relation *relR, Relation *relS,inbet_list *res1,inbet_list *res2){
  int num_of_buckets = pow(2,N);
  Relation *relS_seg , *relR_seg;
  int *histogram_R,*histogram_S,*Psum_R,*Psum_S;
  int i;

  histogram_R = make_histogram(relR);
  Psum_R = make_offsets(histogram_R);
  relR_seg=segmentation(relR,histogram_R,Psum_R);

  histogram_S = make_histogram(relS);
  Psum_S = make_offsets(histogram_S);
  relS_seg=segmentation(relS,histogram_S,Psum_S);

  free(Psum_S);
  Psum_S=make_offsets(histogram_S);
  free(Psum_R);
  Psum_R=make_offsets(histogram_R);

  for(i=0;i<num_of_buckets;i++){
    if(histogram_R[i]<histogram_S[i]){
      IndexAndResult(histogram_R[i],histogram_S[i],Psum_R[i],Psum_S[i],relR_seg,relS_seg,res1,res2);
    }else{
      IndexAndResult(histogram_S[i],histogram_R[i],Psum_S[i],Psum_R[i],relS_seg,relR_seg,res2,res1);
    }
  }
  free(histogram_R);
  free(histogram_S);
  free(Psum_R);
  free(Psum_S);
  free(relS_seg->tuples);
  free(relR_seg->tuples);
  free (relS_seg);
  free (relR_seg);
}

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


void print_relation (Relation* in_relation)
{
  for (int i=0;i<in_relation->num_tuples;i++)
  {
    printf("%ju | %ju \n",in_relation->tuples[i].key,in_relation->tuples[i].payload);
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

Relation *BuildRelation(inbet_list *list,Relation *init_relation){
  /* input: list of inbetween results , initial Relation
      output : a Relation with inbetween tuples*/
  int i;
  Relation *new_relation = (Relation *)malloc(sizeof(Relation));
  new_relation->tuples = malloc(sizeof(tuple)*list->total_tuples);
  new_relation->num_tuples=list->total_tuples;
  inbet_node *current = list->head;
  int node=0,c=0;

  while(current!=NULL){ //for every node in list
    for(i=0;i<current->num_tuples;i++){  //for every rowID in node
      new_relation->tuples[c].key = c;  /*pernao san key to index tis endiamesis domis*/
      new_relation->tuples[c].payload = init_relation->tuples[current->rowIDS[i]].payload;
      c++;
    }
    node++;
    current = current->next;
  }
  return new_relation;
}
