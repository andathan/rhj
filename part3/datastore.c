#include "rhj.h"
#include <string.h>
#include "predicates.h"
#include "inbetween.h"
#include "thpool.h"
#include <unistd.h>
#include <ctype.h>


relation_data **find_corresponding(query *in_query,all_data *datatable){
  /* apo to query ,briskei tis sxeseis pou simmetexoun kai epistrefei enan pinaka apo deiktes sta dedomena
    tis kathe sxesis */
  int j,build_num,counter,num_length=0,corresponding_counter=0;
  relation_data **corresponding_table=NULL;
    //first find how many relations we have
  corresponding_table =(relation_data **)malloc(sizeof(relation_data *)*(in_query->num_of_relations));
  for (j=0;j<=strlen(in_query->sxeseis);j++){
    if (!isspace(in_query->sxeseis[j]) && in_query->sxeseis[j]!=0){
      num_length++;
    }
    else{
      build_num= make_number(num_length, in_query->sxeseis, j);
      num_length=0;
      corresponding_table[corresponding_counter] = datatable->table[build_num];
      corresponding_counter++;
    }
  }
  return corresponding_table;
}


void compute_operation(char op,int constant,Relation *relation,inbet_list *A){
  /*ektelei ta filtra twn query kai eisagi stin lista A ta kleidia*/
  threadpool *thpool = THP_Init(N_THREADS);
  Filter_arg filter_args[N_THREADS];
  int merge = relation->num_tuples / N_THREADS;
  int start=0;
  for(int i=0;i<N_THREADS;i++){
    filter_args[i].constant=constant;
    filter_args[i].op=op;
    filter_args[i].relation = relation;
    filter_args[i].start = start;
    if(i==N_THREADS-1){
      filter_args[i].end = relation->num_tuples;
    }else{
      filter_args[i].end = start+merge;
    }
    start+=merge;

  }
  for(int i=0;i<N_THREADS;i++){
    THP_AddJob(thpool,(void *)FilterJob,&filter_args[i]);
  }
  THP_Barrier(thpool);
  for(int i=0;i<N_THREADS;i++){
    if(A->total_tuples==0){
      A->head = filter_args[i].result->head;
      A->current = filter_args[i].result->current;
    }else{
      A->current->next = filter_args[i].result->head;
      A->current = filter_args[i].result->current;
    }
    A->total_tuples += filter_args[i].result->total_tuples;
  }
  THP_Destroy(thpool);
}


void show_results(inbetween_results *res,relation_data **data, char * token) //thelei ligi doulitsa
{
  /*emfanizei ta athroismata pou zitountai sto query*/
  char *temp=malloc(strlen(token)+1);
  strcpy(temp,token);
  char nl = '\n';
  char space = ' ';
  char strnum[256],output_string[256];
  char *relcol = strtok(temp," \n");
  int rel,col;
  uint64_t col_sum;
  Relation *relation;
  int *keys,no_sum=0;
  while(relcol!=NULL){
    sscanf(relcol,"%d.%d",&rel,&col);
    col_sum=0.0;
    if(res->joined[rel]<=0){
      strcpy(strnum,"NULL ");
    }else{
      relation = BuildRelation(res->inbetween[rel],res->joined[rel],rel,data[rel]->columns[col]);
      for(int i=0;i<relation->num_tuples;i++){
        col_sum+=relation->tuples[i].payload;
      }
      sprintf(strnum,"%ju ",col_sum);
    }
    if(no_sum==0){
      strcpy(output_string,strnum);
    }else{
      strcat(output_string,strnum);
    }
    bzero(strnum,256);
    no_sum++;
    relcol = strtok(NULL," \n");
  }
  output_string[strlen(output_string)-1]='\n';
  write(1,output_string,strlen(output_string));
  bzero(output_string,256);
}

relation_data *parsefile(char * filename){
  uint64_t num=0;
  uint64_t numofColumns;
  uint64_t numofTuples;
  int sum,max,min;
  int size_of_d_table;
  int average;
  FILE *fp = fopen(filename,"rb");
  if (fp==NULL) {
    printf("Can't find file %s.Try another\n",filename);
    return NULL;
  }
  fread(&numofTuples,sizeof(uint64_t),1,fp);
  fread(&numofColumns,sizeof(uint64_t),1,fp);
  relation_data *r2data =(relation_data *)malloc(sizeof(relation_data));
  r2data->columns = (Relation **)malloc(sizeof(Relation *)*numofColumns);
  for(int i=0;i<numofColumns;i++){
    r2data->columns[i] =(Relation *)malloc(sizeof(Relation));
    r2data->columns[i]->tuples = (tuple *)malloc(sizeof(tuple)*numofTuples);
    r2data->columns[i]->num_tuples = numofTuples;
  }
  r2data->numColumns = numofColumns;
//  printf("Has %ju colums\n\n",r2data->numColumns);
  r2data->numTuples = numofTuples;
  char c;
  for(int i=0;i<numofColumns;i++){
    r2data->columns[i]->l = 99999999; //arxikopoihsh se enan megalo arithmo
    r2data->columns[i]->u = 0;
    r2data->columns[i]->f=numofTuples;
    r2data->columns[i]->d=0;
    r2data->columns[i]->restored=0;//this is used as a flag when restoring to its original specifications
    for(int j=0;j<numofTuples;j++){
      fread(&(num),sizeof(uint64_t),1,fp);
      r2data->columns[i]->tuples[j].payload=num;
      r2data->columns[i]->tuples[j].key = j;
      if (num<r2data->columns[i]->l)
      {
        r2data->columns[i]->l=num;
      }
      if (num>r2data->columns[i]->u)
      {
        r2data->columns[i]->u=num;
      }
    }
    //calculating distinct values
      size_of_d_table = (int)r2data->columns[i]->u-(int)r2data->columns[i]->l+1;
       if (size_of_d_table<M)
       {
        r2data->columns[i]->d_table = (char*)calloc(size_of_d_table,sizeof(char));
        for(int j=0;j<numofTuples;j++)
        {
         num = r2data->columns[i]->tuples[j].payload;
          int pos = (int)num - (int)r2data->columns[i]->l;
        r2data->columns[i]->d_table[pos]= 1;
        }
       }
       else
       {
        r2data->columns[i]->d_table = (char*)calloc(M,sizeof(char));
        for(int j=0;j<numofTuples;j++)
        {
         num = r2data->columns[i]->tuples[j].payload;
          int pos = (int)num - (int)r2data->columns[i]->l;
         r2data->columns[i]->d_table[(pos)%M]= 1;
        }
        size_of_d_table = M;
       }
       for(int x=0;x<size_of_d_table;x++)
       {
         if (r2data->columns[i]->d_table[x]==1)
          (r2data->columns[i]->d)++;
       }
//       printf("\n---------\nColumn %d Statistics:\n",i);
//       printf("Low: %ju | Upper %ju | Num Values %f | Distinct Values %f\n---------\n",r2data->columns[i]->l,r2data->columns[i]->u,r2data->columns[i]->f,r2data->columns[i]->d);

  }
  return r2data;
  }
