#include "rhj.h"
#include <string.h>
#include "predicates.h"
#include "inbetween.h"
#include <ctype.h>


relation_data **find_corresponding(query *in_query,all_data *datatable){
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
  int i;
  if (op == '='){
    for (i=0;i<relation->num_tuples;i++){
      if (relation->tuples[i].payload == constant){
        InsertInbetList (A, relation->tuples[i].key);
      }
    }
  }
  else if (op == '>'){
    for (i=0;i<relation->num_tuples;i++){
      if (relation->tuples[i].payload > constant){
        InsertInbetList(A, relation->tuples[i].key);
      }
    }
  }
  else if (op =='<'){
    printf("constant = %d\n",constant );
    for (i=0;i<relation->num_tuples;i++){
      if (relation->tuples[i].payload < constant){
        InsertInbetList (A, relation->tuples[i].key);
      }
    }
  }
}


void show_results(inbetween_results *res,relation_data **data, char * token) //thelei ligi doulitsa
{
  /**/
  char *temp=malloc(strlen(token)+1);
  strcpy(temp,token);
  char *relcol = strtok(temp," \n");
  int rel,col;
  uint64_t col_sum;
  while(relcol!=NULL){
    sscanf(relcol,"%d.%d",&rel,&col);
    col_sum=0.0;
    if(res->joined[rel]<=0){
      printf("NULL ");
    }else{
      for(int i=0;i<res->joined[rel];i++){
        col_sum+=data[rel]->columns[col]->tuples[res->inbetween[rel][i]].payload;
      }
      printf("%ju ",col_sum );
    }
    relcol = strtok(NULL," \n");
  }
  printf("\n");
}

relation_data *parsefile(char * filename){

  uint64_t num=0;
  uint64_t numofColumns;
  uint64_t numofTuples;
  int sum,max,min;
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
  r2data->numTuples = numofTuples;
  char c;
  for(int i=0;i<numofColumns;i++){
    r2data->columns[i]->min = 999999; //arxikopoihsh se enan megalo arithmo
    r2data->columns[i]->max = 0;
    r2data->columns[i]->spread=0;
    for(int j=0;j<numofTuples;j++){
      fread(&(num),sizeof(uint64_t),1,fp);
      r2data->columns[i]->tuples[j].payload=num;
      r2data->columns[i]->tuples[j].key = j;
      if (num<r2data->columns[i]->min)
      {
        r2data->columns[i]->min=num;
      }
      if (num>r2data->columns[i]->max)
      {
        r2data->columns[i]->max=num;
      }
    }
    //calculate column spread
    sum=0;
      for(int j=0;j<numofTuples;j++)
      {
        sum += r2data->columns[i]->tuples[j].payload;
      }
      average = sum/numofTuples;
      for(int j=0;j<numofTuples;j++)
      {
        sum = (r2data->columns[i]->tuples[j].payload - average) * (r2data->columns[i]->tuples[j].payload - average); //sto tetragono
      }
      r2data->columns[i]->spread = sqrt(sum/numofTuples-1);
  }
  return r2data;
  }
