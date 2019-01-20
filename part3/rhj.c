#include "rhj.h"
#include "inbetween.h"
#include "thpool.h"

void HistogramJob(Hist_args* arg){
  int index_bucket,j;
  int num_of_buckets = pow(2,N);
  for(j=0;j<num_of_buckets;j++){
    arg->histogram[j]=0;
  }

  for (j=arg->start; j<arg->end; j++){
    index_bucket = H1(arg->in_relation->tuples[j].payload,arg->mask);
    (arg->histogram[index_bucket])++;
  }
}

void PartitionJob(Partition_args *arg){
  if(arg->in_relation==NULL) return ;
  int index_bucket;   //index_bucket = se pio bucket tha mpei auto to tuple

  for (int j=arg->first; j<arg->last; j++){
    index_bucket=H1(arg->in_relation->tuples[j].payload,arg->mask);
    pthread_mutex_lock(&(arg->partmtx));
    arg->seg_relation->tuples[arg->offsets[index_bucket]+arg->counts[index_bucket]].key=arg->in_relation->tuples[j].key; // rowID,value
    arg->seg_relation->tuples[arg->offsets[index_bucket]+arg->counts[index_bucket]].payload=arg->in_relation->tuples[j].payload;
    (arg->counts[index_bucket])++;
    pthread_mutex_unlock(&(arg->partmtx));
  }
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

void JoinJob(Join_args *arg){
  int j,k,rowID,hash_key,new,previoys,payload1,payload2,range_hashfunction,previous;
  int *chain=NULL , *hashtable=NULL ;
  int key1,key2;
  arg->res1 = InitInbetList();
  arg->res2 = InitInbetList();
  if(arg->index_tuples==0) return;
  int results_c=0;
  range_hashfunction=getnextodd(arg->index_tuples);
  chain = (int *)malloc(sizeof(int)*arg->index_tuples);
  for(k=0;k<arg->index_tuples;k++){
    chain[k] = -1;
  }

  hashtable = (int *)malloc(sizeof(int)*range_hashfunction);
  for (k=0;k<range_hashfunction;k++)
    hashtable[k]=-1;

  /*for every tuple in bucket*/
  for (j=0;j<arg->index_tuples;j++){
    hash_key = H2(arg->indexRel->tuples[arg->index_Psum+j].payload) % range_hashfunction;
    previous = hashtable[hash_key];
    new = j; //pairnw to kainourgio klidi
    hashtable[hash_key] = new;
    if (previous!=-1)
      chain[new] = previous;
  }
  for(j=0;j<arg->comp_tuples;j++){
    hash_key= H2(arg->compRel->tuples[arg->comp_Psum+j].payload) % range_hashfunction;
    rowID = hashtable[hash_key];
    while (rowID!=-1){
      payload1 = arg->indexRel->tuples[arg->index_Psum+rowID].payload;
      payload2 = arg->compRel->tuples[arg->comp_Psum+j].payload;
      if (payload1==payload2){
        key1=(int)arg->indexRel->tuples[arg->index_Psum+rowID].key;
        key2=(int)arg->compRel->tuples[arg->comp_Psum+j].key;
        if(arg->flag==1){
          InsertInbetList(arg->res1,key1);
          InsertInbetList(arg->res2,key2);
        }else{
          InsertInbetList(arg->res2,key1);
          InsertInbetList(arg->res1,key2);
        }
        results_c++;
      }
      rowID = chain[rowID];//pairnw tin proigoumeni eggrafi meso tou chain
    }
  }
  free(hashtable);
  free(chain);
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
  threadpool *thpool = THP_Init(N_THREADS);
  int num_of_buckets = pow(2,N);
  Relation *relS_seg , *relR_seg;
  int *histogram_R,*histogram_S,*Psum_R,*Psum_S;
  inbet_list *res_temp1 = InitInbetList();
  inbet_list *res_temp2 = InitInbetList();

  int i,j;
  int merge = relR->num_tuples / N_THREADS;
  int mask = computemask(N);
  Hist_args histogram_arg[N_THREADS];
  Partition_args *partiotion_arg =(Partition_args *)malloc(sizeof(Partition_args)*num_of_buckets);
  int start = 0;

  relR_seg = malloc(sizeof(Relation));
  relR_seg->tuples = malloc(sizeof(tuple)*relR->num_tuples);
  relR_seg->num_tuples = relR->num_tuples;

  relS_seg = malloc(sizeof(Relation));
  relS_seg->tuples = malloc(sizeof(tuple)*relS->num_tuples);
  relS_seg->num_tuples = relS->num_tuples;


  /*R relation*/
  for(i=0;i<N_THREADS;i++){
    histogram_arg[i].in_relation = relR;
    histogram_arg[i].mask = mask;
    histogram_arg[i].histogram = malloc(sizeof(int)*num_of_buckets);
    histogram_arg[i].start = start;
    if(i==N_THREADS-1){
      histogram_arg[i].end = relR->num_tuples;
    }else{
      histogram_arg[i].end = start+merge;
    }
    start += merge;
  }
  for(i=0;i<N_THREADS;i++){
    THP_AddJob(thpool,(void *)HistogramJob,&histogram_arg[i]);
  }
  THP_Barrier(thpool);    //all histrograms are set
  histogram_R = (int *)malloc(sizeof(int)*num_of_buckets);
  for(j=0;j<num_of_buckets;j++){
    histogram_R[j] = 0;
    for(i=0;i<N_THREADS;i++){
        histogram_R[j] += histogram_arg[i].histogram[j];
     }
   }
   int * counts = malloc(sizeof(int)*num_of_buckets);
   for(j=0;j<num_of_buckets;j++){
    counts[j]=0;
   }
  Psum_R = make_offsets(histogram_R);
  for(i=0;i<num_of_buckets;i++){
    partiotion_arg[i].in_relation = relR;
    partiotion_arg[i].seg_relation = relR_seg;
    partiotion_arg[i].histogram = histogram_R; //apo to job histogram
    partiotion_arg[i].offsets=Psum_R; //apo ti make_offsets
    partiotion_arg[i].counts = counts;
    partiotion_arg[i].mask = mask; //to compute eksw
    partiotion_arg[i].first=Psum_R[i];
    if(i==num_of_buckets-1){
      partiotion_arg[i].last=relR->num_tuples;
    }else{
      partiotion_arg[i].last=Psum_R[i+1];
    }
    pthread_mutex_init(&partiotion_arg[i].partmtx,NULL);
  }
  for(i=0;i<num_of_buckets;i++){
    THP_AddJob(thpool,(void *)PartitionJob,&partiotion_arg[i]);
  }
  THP_Barrier(thpool);

//  relR_seg=segmentation(relR,histogram_R,Psum_R,num_of_buckets);

  /*S relation*/
  start=0;
  merge = relS->num_tuples/N_THREADS;
  for(i=0;i<N_THREADS;i++){
    histogram_arg[i].in_relation = relS;
    histogram_arg[i].mask = mask;
    histogram_arg[i].start = start;
    if(i==N_THREADS-1){
      histogram_arg[i].end = relS->num_tuples;
    }else{
      histogram_arg[i].end = start+merge;
    }
    start += merge;
  }
  for(i=0;i<N_THREADS;i++){
    THP_AddJob(thpool,(void *)HistogramJob,&histogram_arg[i]);
  }
  THP_Barrier(thpool);    //all histrograms are set
  histogram_S = (int *)malloc(sizeof(int)*num_of_buckets);
  for(j=0;j<num_of_buckets;j++){
    histogram_S[j] = 0;
    for(i=0;i<N_THREADS;i++){
        histogram_S[j] += histogram_arg[i].histogram[j];
    }
  }
  Psum_S = make_offsets(histogram_S);
  for(j=0;j<num_of_buckets;j++){
   counts[j]=0;
  }
  for(i=0;i<num_of_buckets;i++){
    partiotion_arg[i].in_relation = relS;
    partiotion_arg[i].seg_relation = relS_seg;
    partiotion_arg[i].histogram = histogram_S; //apo to job histogram
    partiotion_arg[i].offsets=Psum_S; //apo ti make_offsets
    partiotion_arg[i].counts = counts;
    partiotion_arg[i].mask = mask; //to compute eksw
    partiotion_arg[i].first=Psum_S[i];
    if(i==num_of_buckets-1){
      partiotion_arg[i].last=relS->num_tuples;
    }else{
      partiotion_arg[i].last=Psum_S[i+1];
    }
    pthread_mutex_init(&partiotion_arg[i].partmtx,NULL);
  }
  for(i=0;i<num_of_buckets;i++){
    THP_AddJob(thpool,(void *)PartitionJob,&partiotion_arg[i]);
  }
  THP_Barrier(thpool);

//  relS_seg=segmentation(relS,histogram_S,Psum_S,num_of_buckets);
  Join_args *join_arg = malloc(sizeof(Join_args)*num_of_buckets);
  for(i=0;i<num_of_buckets;i++){
    if(histogram_R[i]<histogram_S[i]){
      join_arg[i].index_tuples = histogram_R[i];
      join_arg[i].comp_tuples = histogram_S[i];
      join_arg[i].index_Psum = Psum_R[i];
      join_arg[i].comp_Psum = Psum_S[i];
      join_arg[i].indexRel = relR_seg;
      join_arg[i].compRel = relS_seg;
      join_arg[i].flag=1;
      THP_AddJob(thpool,(void *)JoinJob,&join_arg[i]);
    }else{
      join_arg[i].index_tuples = histogram_S[i];
      join_arg[i].comp_tuples = histogram_R[i];
      join_arg[i].index_Psum = Psum_S[i];
      join_arg[i].comp_Psum = Psum_R[i];
      join_arg[i].indexRel = relS_seg;
      join_arg[i].compRel = relR_seg;
      join_arg[i].flag=2;
      THP_AddJob(thpool,(void *)JoinJob,&join_arg[i]);
    }

  }
  THP_Barrier(thpool);
  /*merge results*/
  for(i=0;i<num_of_buckets;i++){
    if(join_arg[i].res1->total_tuples == 0) continue;
    if(res1->head==NULL){  //first list that we will merge
      res1->head = join_arg[i].res1->head;
      res1->current = join_arg[i].res1->current;
      res1->total_tuples += join_arg[i].res1->total_tuples;
    }else{
      res1->current->next = join_arg[i].res1->head;
      res1->current = join_arg[i].res1->current;
      res1->total_tuples += join_arg[i].res1->total_tuples;
    }
    if(res2->head==NULL){  //first list that we will merge
      res2->head = join_arg[i].res2->head;
      res2->current = join_arg[i].res2->current;
      res2->total_tuples += join_arg[i].res2->total_tuples;
    }else{
      res2->current->next = join_arg[i].res2->head;
      res2->current = join_arg[i].res2->current;
      res2->total_tuples += join_arg[i].res2->total_tuples;
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
  THP_Destroy(thpool);
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

void SerialCompare(Relation *rel1,Relation *rel2,inbet_list *res1,inbet_list *res2){
  for(int i=0;i<rel1->num_tuples;i++){
    if(rel1->tuples[i].payload == rel2->tuples[i].payload ){
      InsertInbetList(res1,rel1->tuples[i].key);
      InsertInbetList(res2,rel2->tuples[i].key);
    }
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

Relation *BuildRelation(int *inb,int num_tuples,int rel_id,Relation *init_relation){
  /* input: list of inbetween results , initial Relation
      output : a Relation with inbetween tuples*/
  int i;
  Relation *new_relation = (Relation *)malloc(sizeof(Relation));
  new_relation->tuples = malloc(sizeof(tuple)*num_tuples);
  new_relation->num_tuples=num_tuples;
  for(i=0;i<num_tuples;i++){
    new_relation->tuples[i].key = i;
    new_relation->tuples[i].payload = init_relation->tuples[inb[i]].payload;
  }
  return new_relation;
}
