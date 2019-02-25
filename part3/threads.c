

typedef struct Hist_args{
  Relation *in_relation;
  int N; //apo tin computemask
  int mask;
  int *histogram = (int *)malloc(sizeof(int)*pow(2,N));
  int start;
  int end;
}Hist_args;

typedef struct Partition_args{
  Relation *in_relation;
  int *histogram; //apo to job histogram
  int *offsets; //apo ti make_offsets
  int mask; //to compute eksw
  Relation* seg_relation=(Relation*)malloc(sizeof(Relation));
  seg_relation->tuples =(tuple*)malloc(sizeof(tuple)*seg_relation->num_tuples);
  pthread_mutex_t partmtx;
}Partition_args;

void thread_Hist(Hist_args* arg){

  int index_bucket,j;
  for(j=0;j<pow(2,N);j++){
    arg->histogram[j]=0;
  }
  for (j=start; j<end; j++){
    index_bucket = H1(arg->in_relation->tuples[j].payload,arg->mask);
    arg->histogram[index_bucket]++;
  }
}

void thread_partition(Partition_args *p){

  if(p->in_relation==NULL) return NULL;
  int index_bucket;   //index_bucket = se pio bucket tha mpei auto to tuple
  p->seg_relation->num_tuples=p->in_relation->num_tuples;

  for (int j=0; j<p->in_relation->num_tuples; j++){
    index_bucket=H1(p->in_relation->tuples[j].payload,p->mask);
    pthread_mutex_unlock(&(p->partmtx));
    p->seg_relation->tuples[p->offsets[index_bucket]].key=p->in_relation->tuples[j].key; // rowID,value
    p->seg_relation->tuples[p->offsets[index_bucket]].payload=p->in_relation->tuples[j].payload;
    (p->offsets[index_bucket])++;
    pthread_mutex_lock(&(p->partmtx));
  }
}
