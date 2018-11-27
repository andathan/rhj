#include "rhj.h"
#include <string.h>
#include "predicates.h"
#include "inbetween.h"
#include <ctype.h>


relation_data **find_corresponding(char * token,all_data *datatable,int *num_relations)
{
  int j,build_num,counter,num_length,corresponding_counter=0;
  relation_data **corresponding_table=NULL;
    //first find how many relations we have
  *num_relations=1;
  for (j=0;j<=strlen(token);j++)
  {
    if (isspace(token[j]))
    {
      (*num_relations)++;
    }
  }
  corresponding_table =(relation_data **)malloc(sizeof(relation_data *)*(*num_relations));
  for (j=0;j<=strlen(token);j++)
  {
    if (!isspace(token[j]) && token[j]!=0)
    {
      num_length++;
    }
    else
    {
      build_num= make_number(num_length, token, j);
      num_length=0;
      corresponding_table[corresponding_counter] = datatable->table[build_num];
      corresponding_counter++;
    }
  }
  return corresponding_table;
}

void compute_operation(char op,int constant,Relation *relation,inbet_list *A)
{
  int i;
  if (op == '=')
  {
    for (i=0;i<relation->num_tuples;i++)
    {
      if (relation->tuples->payload == constant)
      {
        UpdateInbetList (A, relation->tuples->key);
      }
    }
  }
  else if (op == '>')
  {
    for (i=0;i<relation->num_tuples;i++)
    {
      if (relation->tuples->payload > constant)
      {
        UpdateInbetList(A, relation->tuples->key);
      }
    }
  }
  else if (op =='<')
  {
    for (i=0;i<relation->num_tuples;i++)
    {
      if (relation->tuples->payload < constant)
      {
        UpdateInbetList (A, relation->tuples->key);
      }
    }
  }
}

int sum(inbet_list *list,Relation *rel){
  /*computes the summary of the relation rel according to keys in list  *
   *  returns sum                                                         */
   int i,sum=0;
  inbet_node *current = list->head;
  while(current!=NULL){
    /*for every key in list , add its value to sum*/
    for(i=0;i<current->num_tuples;i++){
      sum += rel->tuples[current->rowIDS[i]].payload;
    }
    current=current->next;
  }
  return sum;
}


void show_results(inbetween_results *res,all_data *data, char * token) //thelei ligi doulitsa
{
  /**/
  char *relcol = strtok(token," ");
  int rel,col,col_sum;
  while(relcol!=NULL){
    sscanf(relcol,"%d.%d",&rel,&col);
    if(res->inbet_lists[rel]->head==NULL){
      printf("NULL\n");
    }else{
      col_sum = sum(res->inbet_lists[rel],data->table[rel]->columns[col]);
      printf("~%d\n",col_sum );
    }
    relcol = strtok(NULL," ");
  }
}

relation_data *parsefile(char * filename){

  uint64_t num;
  uint64_t numofColumns;
  uint64_t numofTuples;

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
  }
  printf("%ju\n",numofTuples );
  printf("%ju\n",numofColumns );
  for(int i=0;i<numofColumns;i++){
    for(int j=0;j<numofTuples;j++){
      fread(&(r2data->columns[i]->tuples[j].payload),sizeof(int32_t),1,fp);
      r2data->columns[i]->tuples[j].key = j;
    }
  }
  /*for(int i=0;i<numofColumns;i++){
    for(int j=0;j<numofTuples;j++){
      printf("%d |%d\n", r2data->columns[i]->tuples[j].key,r2data->columns[i]->tuples[j].payload);
    }
  }*/
  return r2data;
}