#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "predicates.h"

batch *InitBatch(){
  batch *my_batch=malloc(sizeof(batch));
  my_batch->num_of_queries=0;
}

batch *AddToBatch(batch *my_batch,char **tokens){
  int num_predicates;
  int num_relations=1;
  for(int j=0;j<=strlen(tokens[0]);j++){
    if (isspace(tokens[0][j])){
      num_relations++;
    }
  }
  my_batch->queries_table = realloc(my_batch->queries_table,sizeof(query*)*(my_batch->num_of_queries+1));
  my_batch->queries_table[my_batch->num_of_queries] = (query *)malloc(sizeof(query));
  my_batch->queries_table[my_batch->num_of_queries]->sxeseis = tokens[0];
  printf("sxeseis %s\n",tokens[0] );
  my_batch->queries_table[my_batch->num_of_queries]->proboles = tokens[2];
  num_predicates = find_num_of_predicates(tokens[1]);
  my_batch->queries_table[my_batch->num_of_queries]->num_of_predicates = num_predicates;
  my_batch->queries_table[my_batch->num_of_queries]->katigorimata  = fill_predicates(tokens[1],num_predicates);
  my_batch->queries_table[my_batch->num_of_queries]->num_of_relations = num_relations;
  my_batch->num_of_queries++;
}

inbetween_results *execute_predicate(predicates *pred,relation_data **relations,inbetween_results *inb_res){
  Relation *relation,*rel1,*rel2;
  inbet_list *result1,*result2;
  if(pred->rel1==-1){
    rel2 = relations[pred->rel2]->columns[pred->col2];
    if(inb_res->inbet_lists[pred->rel2]->head==NULL){
      relation = rel2;
    }else{
      relation = BuildRelation(inb_res->inbet_lists[pred->rel2],rel2);
    }
    result2 = InitInbetList();
    compute_operation(pred->op,pred->col1,relation,result2);
    inb_res = UpdateInbetList2(inb_res,result2,pred->rel2);
  }else if(pred->rel2==-1){
    rel1 = relations[pred->rel1]->columns[pred->col1];
    if(inb_res->inbet_lists[pred->rel1]->head==NULL){
      relation = rel1;
    }else{
      relation = BuildRelation(inb_res->inbet_lists[pred->rel1],rel1);
    }
    result1 = InitInbetList();
    compute_operation(pred->op,pred->col2,relation,result1);
    inb_res = UpdateInbetList2(inb_res,result1,pred->rel1);
  }else if(pred->rel1==pred->rel2){
    printf("%d==%d\n", pred->rel1,pred->rel2);
    //edo prepei na mpei isotita pinakwn - kalitera se sinartisi
  }else{
    rel1 = relations[pred->rel1]->columns[pred->col1];
    rel2 = relations[pred->rel2]->columns[pred->col2];
    if(inb_res->inbet_lists[pred->rel2]->head!=NULL){
      rel2 = BuildRelation(inb_res->inbet_lists[pred->rel2],rel2);
    }
    if(inb_res->inbet_lists[pred->rel1]->head!=NULL){
      rel1 = BuildRelation(inb_res->inbet_lists[pred->rel1],rel1);
    }
    result1 = InitInbetList();
    result2 = InitInbetList();
    if(pred->op=='='){
      RadixHashJoin(rel1,rel2,result1,result2);
      inb_res=UpdateInbetList(inb_res,result1,result2,pred->rel1,pred->rel2);
    }else{
      //edo prepei na mpoun ta > , < metaksi pinakwn - sinartisi
      printf("%c\n",pred->op );
    }
  }
  return inb_res;
}


void execute_query(query *in_query,all_data *data){
  relation_data **relations = find_corresponding(in_query,data);
  int next_pred ;
  inbetween_results *inb_res=InitInbetResults(in_query->num_of_relations);
  for(int i=0;i<in_query->num_of_predicates;i++){
    next_pred = select_predicate(in_query->num_of_predicates,in_query->katigorimata,relations);
    inb_res = execute_predicate(in_query->katigorimata[next_pred],relations,inb_res);
    in_query->katigorimata[next_pred]->op= '.';
  }
  show_results(inb_res,relations,in_query->proboles);
  //free inb_res , relations
}


void seperate_predicate (char * input, char * temp_tokens[3] )
{
  char *tokens[3];
  tokens[0] = strtok(input,"|");
  temp_tokens[0] = malloc(strlen(tokens[0])+1);
  strcpy(temp_tokens[0],tokens[0]);
  tokens[1] = strtok(NULL,"|");
  temp_tokens[1] = malloc(strlen(tokens[1])+1);
  strcpy(temp_tokens[1],tokens[1]);
  tokens[2] = strtok(NULL,"|");
  temp_tokens[2] = malloc(strlen(tokens[2])+1);
  strcpy(temp_tokens[2],tokens[2]);
}

int find_num_of_predicates (char * token)
{
  int i,num_predicates=0;
  for(i=0;i<strlen(token)-3;i++)//den xreiazetai na koitaksoume stis teleutaies 3 theseis tou string gia &
  {
    if(token[i] == '&')
    num_predicates++;
  }
  return ++num_predicates; //exoume +1 apo ton arithmo & pou iparxoun predicates
}

void print_predicates (predicates ** predicates,int num_predicates)
{
  int i;
  for (i=0;i<num_predicates;i++)
    {
      if (predicates[i]->rel1!=-1 && predicates[i]->rel2!=-1)
      printf("%d.%d%c%d.%d\n",predicates[i]->rel1,predicates[i]->col1,predicates[i]->op,predicates[i]->rel2,predicates[i]->col2);
      else if (predicates[i]->rel1==-1)
      printf("%d%c%d.%d\n",predicates[i]->col1,predicates[i]->op,predicates[i]->rel2,predicates[i]->col2);
      else if (predicates[i]->rel2==-1)
      printf("%d.%d%c%d\n",predicates[i]->rel1,predicates[i]->col1,predicates[i]->op,predicates[i]->col2);
    }
}

int make_number (int num_length,char * compute_num, int j)
{
  int k=1;
  int build_num=0;
  for (k=num_length;k>1;k--)
  {
    build_num+= pow(10,(k-1))*(compute_num[j-k]-'0');
  }
  build_num+= compute_num[j-k]-'0';
  return build_num;
}

predicates **fill_predicates (char * token, int num_predicates)
{
  char flag;
  int i,j,k,num_of_relations_left,num_of_relations_right,num_length=0,build_num=0;
  char * compute_num,* query;
  predicates **pred = malloc(sizeof(predicates *)*num_predicates);

  printf("num of pred %d\n",num_predicates);

  query = strtok(token,"&");
  for (i=0;i<num_predicates;i++)
  {
    pred[i]=malloc(sizeof(predicates));
    num_of_relations_left=0;
    num_of_relations_right=0;
    num_length=0;
    flag=0;
    compute_num = query;
    printf("Checking query %s\n",query);
    for(j=0;j<strlen(compute_num);j++)
    {
      if (compute_num[j] == ' ')
      {
        continue;
      }
      if(compute_num[j]>='0' && compute_num[j]<='9')
        num_length++;
      if (compute_num[j] == '>' || compute_num[j] == '<' || compute_num[j] == '=')
      {
        build_num = make_number(num_length, compute_num, j);
        num_length=0;
        pred[i]->col1= build_num;
        if (num_of_relations_left==0) //exoume apla metabliti aristera
        {
          pred[i]->rel1=-1;
        }
        flag=1;
        printf("i=%d\n",i );
        pred[i]->op = compute_num[j];
      }
      if(compute_num[j] == '.' && flag==0)
      {
        build_num = make_number(num_length, compute_num, j);
        num_length=0;
        if (num_of_relations_left==0)
          pred[i]->rel1=build_num;
        else
          pred[i]->col1=build_num;
        num_of_relations_left++;
        }
      else if (compute_num[j]=='.' && flag==1)
      {
        build_num = make_number(num_length, compute_num, j);
        num_length=0;
        if (num_of_relations_right==0)
          pred[i]->rel2=build_num;
        else
          pred[i]->col2=build_num;
        num_of_relations_right++;
      }
    }
    build_num = make_number(num_length, compute_num, j);
    num_length=0;
    pred[i]->col2=build_num;
    if (num_of_relations_right==0)//exoume apla metabliti deksia
    {
      pred[i]->rel2=-1;
    }
    query = strtok(NULL,"&");
  }
  return pred;
}

int select_predicate(int numofPredicates, predicates ** input_predicates,relation_data ** datatable){
//girnaei ton arithmo me to predicate pou prepei na ektelesoume prota. theloume prota auta pou tha bgoun grigora
  /*
    epistrefei to predicate pou tha ektelestei. ekteloume prota ta filtra.
    apo ta filtra dinoume perissotero baros se auta pou exoun megalo ginomeno diaspora * apostasi teleutaias timis apo to filtro.
     meta ta joins
  */
  int i,return_predicate=-1,weight,constant,distance_to_filter,rel,col,max,min;
  int best=0;
  int min_weight=-1;
  char no_more_filters=1;
  char select_filters = 1;
  for (int j=0;j<numofPredicates;j++){
    if ((input_predicates[j]->rel1 == -1 || input_predicates[j]->rel2 == -1) && input_predicates[j]->op!='.'){
      no_more_filters = 0;
      break;
    }
  }
  if (no_more_filters==1)//teleiosame me ta filters, pame sta joins
    select_filters = 0;
  for (i=0;i<numofPredicates;i++){
    if (select_filters==1){
      if ((input_predicates[i]->rel1 == -1 || input_predicates[i]->rel2 == -1) && input_predicates[i]->op != '.'  ){ //einai filtro && den exei ektelestei
        if (input_predicates[i]->rel1==-1){
          constant = input_predicates[i]->col1;
          rel = input_predicates[i]->rel2;
          col = input_predicates[i]->col2;
        }else{
          constant = input_predicates[i]->col2;
          rel = input_predicates[i]->rel1;
          col = input_predicates[i]->col1;
        }
        if (input_predicates[i]->op == '<'){
          distance_to_filter =   datatable[rel]->columns[col]->max - constant;
          weight = datatable[rel]->columns[col]->spread  * distance_to_filter;
        }else if (input_predicates[i]->op =='>'){
          distance_to_filter = datatable[rel]->columns[col]->min -constant;
          weight = datatable[rel]->columns[col]->spread  * distance_to_filter;
        }else{
          weight = datatable[rel]->columns[col]->spread;
        }
        if (distance_to_filter<0){ // theloume apoliti timi
          distance_to_filter= distance_to_filter *(-1);
        }
        if (weight<min_weight || min_weight==-1){//an exoume proti epanalipsi
          min_weight = weight;
          return_predicate = i;
        }
      }
    }
    if (select_filters == 0){//den exw alla filtra h exw metaksi sxeseon
      if ((input_predicates[i]->rel1 != -1 &&  input_predicates[i]->rel2 != -1) && input_predicates[i]->op != '.' ){ // join pou den exei ginei
            return_predicate = i;
      }
    }
  }
  return return_predicate;
}

void FreeBatch(batch *b){
  if(b==NULL) return ;
  for(int k=0;k<b->num_of_queries;k++){
    free(b->queries_table[k]->sxeseis);
    free(b->queries_table[k]->proboles);
    for(int i;i<b->queries_table[k]->num_of_predicates;i++){
      free(b->queries_table[k]->katigorimata[i]);
    }
    free(b->queries_table[k]->katigorimata);
  }
  free(b);
}
