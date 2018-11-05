#include "rhj.h"

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


Result *IndexAndResult(int index_tuples,int comp_tuples,int index_Psum,int comp_Psum,Relation *indexRel,Relation *compRel,Relation *initialRel,Result *results){

  int j,k,rowID,hash_key,new,previoys,key1,key2,payload1,payload2,range_hashfunction,previous;
  int *chain=NULL , *hashtable=NULL;

  if(index_tuples==0) return results;

  range_hashfunction=getnextodd(index_tuples);

  chain = (int *)malloc(sizeof(int)*index_tuples);
  for(k=0;k<index_tuples;k++)
    chain[k] = -1;

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
        key1=indexRel->tuples[index_Psum+rowID].key;
        key2=compRel->tuples[comp_Psum+j].key;
        InsertResult(results,key1,key2);
      }
      rowID = chain[rowID];//pairnw tin proigoumeni eggrafi meso tou chain
    }
  }
  free(hashtable);
  free(chain);
  return results;
}


Result* RadixHashJoin(Relation *relR, Relation *relS){
  int num_of_buckets = pow(2,N);
  Relation *relS_seg , *relR_seg;
  int *histogram_R,*histogram_S,*Psum_R,*Psum_S;
  int i;

  Result *results = ListInit();

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

  for (i=0;i<num_of_buckets;i++){
    if (histogram_R[i]<histogram_S[i]){
      results = IndexAndResult(histogram_R[i],histogram_S[i],Psum_R[i],Psum_S[i],relR_seg,relS_seg,relR,results);
    }else{
      results = IndexAndResult(histogram_S[i],histogram_R[i],Psum_S[i],Psum_R[i],relS_seg,relR_seg,relS,results);
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
  return results;
}
