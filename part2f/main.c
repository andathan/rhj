#include "inbetween.h"
#include "rhj.h"
#include "predicates.h"
#include <string.h>

int main (void)
{
  inbetween_results *inb_results = NULL;
  Relation * relation,* relation2;
  char * tokens[3];
  int i;
  char input[80] = "11 0 5|0.2=1.0&1.0=2.2&0.1=5784|2.3 0.1 0.1";
  char *filename=NULL;
  size_t linesize;

  all_data *datatable =(all_data *)malloc(sizeof(all_data));
  datatable->num_relations=0;
  datatable->table=NULL;
//  datatable->table = (relation_data **)malloc(sizeof(relation_data *));

  getline(&filename,&linesize,stdin);
  while(strcmp(filename,"done\n")!= 0){
    datatable->table = realloc(datatable->table,sizeof(relation_data *)*(datatable->num_relations+1));
    filename[strlen(filename)-1]='\0';
    datatable->table[datatable->num_relations] = parsefile(filename);
    datatable->num_relations++;
    getline(&filename,&linesize,stdin);
  }

  relation_data ** arrays_in_query;
  int num_predicates , num_relations_in_query;
  predicates **predicates_table;

  seperate_predicate(input,tokens);
  printf("1:%s\n2:%s\n3:%s\n",tokens[0],tokens[1],tokens[2]);
  int r1,r2,c1,c2;
  inbet_list *List1,*List2=NULL;
  Relation *rel1,*rel2;
  arrays_in_query= find_corresponding(tokens[0],datatable,&num_relations_in_query);
  printf("!! %d\n",num_relations_in_query );
  inb_results = InitInbetResults(num_relations_in_query);
  num_predicates = find_num_of_predicates(tokens[1]);

  for(int i=0;i<num_relations_in_query;i++){
    //printf("Relation %d \n",i);
    for(int j=0;j<arrays_in_query[i]->numColumns;j++){
      for(int k=0;k<arrays_in_query[i]->numTuples;k++){
      //  printf("%ju|",arrays_in_query[i]->columns[j]->tuples[k].payload );
      }
    }
  }

  printf("We have %d predicate\n",num_predicates);
  inbet_list *result1,*result2=NULL;
  predicates_table = fill_predicates(tokens[1],num_predicates);
  print_predicates(predicates_table,num_predicates);
  int constant;
  //apo edw kai katw untested kodikas
  for(i=0;i<num_predicates;i++)
  {
    printf("predicate : %d.%d%c%d.%d\n",predicates_table[i]->rel1,predicates_table[i]->col1,predicates_table[i]->op,predicates_table[i]->rel2,predicates_table[i]->col2);
    r1 =  predicates_table[i]->rel1;
    r2 = predicates_table[i]->rel2;
    c1 =  predicates_table[i]->col1;
    c2 =  predicates_table[i]->col2;

    if(r1==-1){
      constant = predicates_table[i]->col1;
      List2=inb_results->inbet_lists[r2];
      rel2 = arrays_in_query[r2]->columns[c2];

      if(List2->head==NULL){
        relation = rel2;
      }else{
        relation = BuildRelation(List2,rel2);
      }
      result2 = InitInbetList();
      compute_operation(predicates_table[i]->op,constant,relation,result2);
      inb_results = UpdateInbetList2(inb_results,result2,r2);
    }else if(r2==-1){
      constant = predicates_table[i]->col2;
      List1=inb_results->inbet_lists[r1];
      rel1 = arrays_in_query[r1]->columns[c1];
      printf("rel 1 = %d , const = %d \n", predicates_table[i]->rel1,predicates_table[i]->col2);
      if(List1->head==NULL){
        printf("relation %d has no previous results\n",predicates_table[i]->rel1);
        relation = rel1;
      }else{
        relation = BuildRelation(List1,rel1);
        printf("relation %d has previous results\n",predicates_table[i]->rel1);
        printf("BuildRelation\n");
      }
      result1 = InitInbetList();
      compute_operation(predicates_table[i]->op,constant,relation,result1);
      inb_results = UpdateInbetList2(inb_results,result1,r1);
    }else if(predicates_table[i]->rel1==predicates_table[i]->rel2){
      printf("%d==%d\n", predicates_table[i]->rel1,predicates_table[i]->rel2);
      //edo prepei na mpei isotita pinakwn - kalitera se sinartisi
    }else{
      List1=inb_results->inbet_lists[r1];
      List2=inb_results->inbet_lists[r2];
      rel1 = arrays_in_query[r1]->columns[c1];
      rel2 = arrays_in_query[r2]->columns[c2];
      result1 = InitInbetList();
      result2 = InitInbetList();
      if(List2->head!=NULL){
        rel2 = BuildRelation(List2,rel2);
        printf("list2 has been reiinited\n" );
        printf("relation %d has no previous results\n",predicates_table[i]->rel2);
        printf("BuildRelation\n");
      }
      if(List1->head!=NULL){
        printf(" !! %d ,%d\n",r1,List1->head->rowIDS[0] );
        rel1 = BuildRelation(List1,rel1);
        printf("relation %d has previous results\n",predicates_table[i]->rel1);
        printf("BuildRelation\n");
      }
      if(predicates_table[i]->op=='='){
        printf("kano join\n" );
        RadixHashJoin(rel1,rel2,result1,result2);
        inb_results=UpdateInbetList(inb_results,result1,result2,r1,r2);
        printf("---> %d\n",inb_results->inbet_lists[r2]->joined );
      }else{
        //edo prepei na mpoun ta > , < metaksi pinakwn - sinartisi
        printf("%c\n",predicates_table[i]->op );
      }
    }
  }
  show_results(inb_results,arrays_in_query,tokens[2]); //thelei ligi doulitsa.
  free (arrays_in_query); //to coressponding table xriazete?
}
