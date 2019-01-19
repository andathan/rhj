#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "predicates.h"
#include "rhj.h"
#include "trees.h"

float instead_of_pow(float a, float b)
{
  return 0.1;
}
predicates * make_predicate (int rel1, int col1, int rel2, int col2, char op)
{
  predicates * new_pred = (predicates*)malloc(sizeof(predicates));
  new_pred->rel1 = rel1;
  new_pred->col1= col1;
  new_pred->op = op;
  new_pred->rel2=rel2;
  new_pred->col2=col2;
}
void restore_statistics (relation_data ** relations, int rel)
{
  int i,k;
  relation_data * r2data;
  if (rel!=-1)
    r2data = relations[rel];

  for (k=0;k<r2data->numColumns;k++)
  {
    r2data->columns[k]->l = r2data->columns[k]->prev_l;
    r2data->columns[k]->u = r2data->columns[k]->prev_u;
    r2data->columns[k]->f = r2data->columns[k]->prev_f;
    r2data->columns[k]->d = r2data->columns[k]->prev_d;
    r2data->columns[k]->restored=0;
  }

}
float update_statistics(relation_data ** relations, predicates ** predicate, int pred_num){
  //first keep old values in case we need to restore them
  //for table A
  predicates * curr_pred;
  curr_pred = (predicates*)malloc(sizeof(predicates));
  curr_pred->rel1 = predicate[pred_num]->rel1;
  curr_pred->col1 = predicate[pred_num]->col1;
  curr_pred->rel2 = predicate[pred_num]->rel2;
  curr_pred->col2 = predicate[pred_num]->col2;
  curr_pred->op = predicate[pred_num]->op;
  int curr_column = curr_pred->col1;
  int k;
  float old_d,second_old_d;
  relation_data * r2data,* second_r2data;
  if (curr_pred->rel1!=-1)
    {r2data = relations[curr_pred->rel1];
//      printf("Curr pred rel 2 is %d with col2 %d\n",curr_pred->rel2,curr_pred->col2);
    }
//    else
//    printf("its just -1");
  if (curr_pred->rel2!=-1)
    {
    second_r2data = relations[curr_pred->rel2];
//    printf("\nPaw na tou zitiso ths rel %d to col %d\n",curr_pred->rel2,curr_pred->col2);
//    printf("d is %f\n",second_r2data->columns[curr_pred->col2]->d);
    }
  else
    second_r2data = NULL;
  int i = curr_column;
//  printf("Asking for %d of %d\n\n",curr_pred->rel1, curr_column);
  float n = (r2data->columns[i]->u - r2data->columns[i]->l +1);
  float old_f= r2data->columns[i]->f;//used for storing previous f value
  for (k=0;k<r2data->numColumns;k++)
  {
    if (r2data->columns[k]->restored==0)//store data only if its the first call. this protects that from storing intermediate results in the restoring section
    {
    r2data->columns[k]->prev_l =r2data->columns[k]->l;
    r2data->columns[k]->prev_u =r2data->columns[k]->u;
    r2data->columns[k]->prev_f =r2data->columns[k]->f;
    r2data->columns[k]->prev_d =r2data->columns[k]->d;
    r2data->columns[k]->restored=1;
    }
  }
  if (curr_pred->rel2==-1 && curr_pred->op=='=')//R.A = k
  {
    if (r2data->columns[i]->u-r2data->columns[i]->l+1<M) //we have a tabe of size u- l+1
    {
      if (curr_pred->col2 < r2data->columns[i]->l || curr_pred->col2 > r2data->columns[i]->u )
      {
        r2data->columns[i]->d=0.0;
        r2data->columns[i]->f=0.0;
      }
      else if (r2data->columns[i]->d_table[(int)(curr_pred->col2 - r2data->columns[i]->l)]==1)
      {
  //      printf("%d exists in d_table\n",curr_pred->col2);
        r2data->columns[i]->f=r2data->columns[i]->f/r2data->columns[i]->d;
        r2data->columns[i]->d=1.0;
      }
      else
        {
          r2data->columns[i]->d=0.0;
          r2data->columns[i]->f=0.0;
        }
    }
    else //we have a table of size M
    {
      if (r2data->columns[i]->d_table[(int)(curr_pred->col2-r2data->columns[i]->u)%M]==1)
        {
  //        printf("%d exists in d_table\n",curr_pred->col2);
          r2data->columns[i]->f=r2data->columns[i]->f/r2data->columns[i]->d;
          r2data->columns[i]->d=1.0;
        }
        else
          {
            r2data->columns[i]->d=0.0;
            r2data->columns[i]->f=0.0;
          }
    }
    r2data->columns[i]->l = curr_pred->col2;
    r2data->columns[i]->u = curr_pred->col2;
    //now for the rest columns
    for (i=0;i<r2data->numColumns;i++)
    {
      if (i==curr_column)
        continue;
        r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -instead_of_pow((1.0- (r2data->columns[curr_column]->f/old_f)),r2data->columns[i]->f/r2data->columns[i]->d));
        r2data->columns[i]->f = r2data->columns[curr_column]->f;
    }
  }
  else if (curr_pred->rel2 == -1 && curr_pred->op=='>')//R.A > k
  {
    float k1 = curr_pred->col2;
    if (k1<r2data->columns[i]->l)
        k1 = r2data->columns[i]->l;
    r2data->columns[i]->l =k1;
    r2data->columns[i]->d = r2data->columns[i]->d * (r2data->columns[i]->u-k1)/(r2data->columns[i]->u-r2data->columns[i]->l);
    r2data->columns[i]->f = r2data->columns[i]->f * (r2data->columns[i]->u-k1)/(r2data->columns[i]->u-r2data->columns[i]->l);
    //now for the rest colmns
    for (i=0;i<r2data->numColumns;i++)
    {
      if (i==curr_column)
        continue;
      r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -instead_of_pow((1.0- (r2data->columns[curr_column]->f/old_f)),r2data->columns[i]->f/r2data->columns[i]->d));
      r2data->columns[i]->f = r2data->columns[curr_column]->f;
    }
  }
  else if (curr_pred->rel2== -1 && curr_pred->op=='<')//R.A < k
  {
    float k2 = curr_pred->col2;
    if (k2>r2data->columns[i]->u)
      k2 = r2data->columns[i]->u;
    r2data->columns[i]->u = k2;
    r2data->columns[i]->d = r2data->columns[i]->d * (k2-r2data->columns[i]->l)/(r2data->columns[i]->u-r2data->columns[i]->l);
    r2data->columns[i]->f = r2data->columns[i]->f * (k2-r2data->columns[i]->l)/(r2data->columns[i]->u-r2data->columns[i]->l);
    //now for the rest colums
    for (i=0;i<r2data->numColumns;i++)
    {
      if (i==curr_column)
        continue;
      r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -instead_of_pow((1.0- (r2data->columns[curr_column]->f/old_f)),r2data->columns[i]->f/r2data->columns[i]->d));
      r2data->columns[i]->f = r2data->columns[curr_column]->f;
    }
}
  else if (curr_pred->rel1 == curr_pred->rel2)//R.A = R.B
  {
    if (r2data->columns[i]->l<r2data->columns[curr_pred->col2]->l)
      {
        r2data->columns[i]->l = r2data->columns[curr_pred->col2]->l;
      }
    else
      {
        r2data->columns[curr_pred->col2]->l = r2data->columns[i]->l;
      }
    if (r2data->columns[i]->u<r2data->columns[curr_pred->col2]->u)
    {
      r2data->columns[curr_pred->col2]->u = r2data->columns[i]->u;
    }
    else
    {
      r2data->columns[i]->u = r2data->columns[curr_pred->col2]->u;
    }
    r2data->columns[i]->f = r2data->columns[i]->f/n;
    r2data->columns[curr_pred->col2]->f =  r2data->columns[i]->f;
    r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -instead_of_pow((1.0- (r2data->columns[curr_column]->f/old_f)),(r2data->columns[i]->f/r2data->columns[i]->d)));
    r2data->columns[curr_pred->col2]->d = r2data->columns[i]->d;
    for (i=0;i<r2data->numColumns;i++)
    {
      if (i==curr_column)
        continue;
      r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -instead_of_pow((1.0- (r2data->columns[curr_column]->f/old_f)),r2data->columns[i]->f/r2data->columns[i]->d));
      r2data->columns[i]->f = r2data->columns[curr_column]->f;
    }
  }
  else if ((curr_pred->rel1 == curr_pred->rel2) && (curr_pred->col1 == curr_pred->col2)) //autosisxetisi
  {
    r2data->columns[i]->f = r2data->columns[i]->f * r2data->columns[i]->f /n;
    for (i=0;i<r2data->numColumns;i++)
    {
      if (i==curr_column)
        continue;;
      r2data->columns[i]->f = r2data->columns[curr_column]->f;
    }
  }
  else //join metaksi 2 pinakon
  {

    //for table B
    for (k=0;k<second_r2data->numColumns;k++)
    {
      if (second_r2data->columns[k]->restored==0)//store data only if its the first call. this protects that from storing intermediate results in the restoring section
      {
      second_r2data->columns[k]->prev_l =second_r2data->columns[k]->l;
      second_r2data->columns[k]->prev_u =second_r2data->columns[k]->u;
      second_r2data->columns[k]->prev_f =second_r2data->columns[k]->f;
      second_r2data->columns[k]->prev_d =second_r2data->columns[k]->d;
      second_r2data->columns[k]->restored=1;
      }
    }
    //dialego apo ta 2 low to megalitero low, kai apo ta 2 upper to mikrotero upper
    if (r2data->columns[i]->l<second_r2data->columns[curr_pred->col2]->l)
    {
      r2data->columns[i]->l = second_r2data->columns[curr_pred->col2]->l;
    }
    else
    {
      second_r2data->columns[curr_pred->col2]->l = r2data->columns[i]->l;
    }
    if (r2data->columns[i]->u<second_r2data->columns[curr_pred->col2]->u)
    {
      second_r2data->columns[curr_pred->col2]->u = r2data->columns[i]->u;
    }
    else
    {
      r2data->columns[i]->u = second_r2data->columns[curr_pred->col2]->u;
    }
    float n = (float)(r2data->columns[i]->u - r2data->columns[i]->l +1);
    r2data->columns[i]->f= r2data->columns[i]->f * second_r2data->columns[curr_pred->col2]->f/n;
    second_r2data->columns[curr_pred->col2]->f = r2data->columns[i]->f;
    old_d = r2data->columns[i]->d;
    r2data->columns[i]->d = r2data->columns[i]->d  * second_r2data->columns[curr_pred->col2]->d/n;
    second_old_d = second_r2data->columns[curr_pred->col2]->d;
    second_r2data->columns[curr_pred->col2]->d = r2data->columns[i]->d;
    //now for the rest columns
    //for table A
    for (i=0;i<r2data->numColumns;i++)
    {
      if (i==curr_column)
        continue;
  //    printf("\nd of i column is %f| d of curr_column %f| old_d is %f| f of i columns is %f | d of i column is %f\n", r2data->columns[i]->d , r2data->columns[curr_column]->d,old_d,r2data->columns[i]->f,r2data->columns[i]->d);
      r2data->columns[i]->d = r2data->columns[i]->d * (1.0 - instead_of_pow((1.0 - (r2data->columns[curr_column]->d/old_d)),r2data->columns[i]->f/r2data->columns[i]->d));
  //   printf("Thus, new d is %f\n",r2data->columns[i]->d );
      r2data->columns[i]->f = r2data->columns[curr_column]->f;
    }
    //for table B
    for (i=0;i<second_r2data->numColumns;i++)
    {
  //    printf("Num of colums is %ju and i is %d\n",second_r2data->numColumns,i);
      if (i==curr_pred->col2)
        continue;
  //    printf("(second) d of i column is %f| d of curr_column %f| old_d is %f| f of i columns is %f | d of i column is %f, instead_of_pow is %f\n", second_r2data->columns[i]->d,second_r2data->columns[curr_column]->d,second_old_d,second_r2data->columns[i]->f,second_r2data->columns[i]->d,instead_of_pow((1.0 - (second_r2data->columns[curr_column]->d/second_old_d)),second_r2data->columns[i]->f/second_r2data->columns[i]->d));
      second_r2data->columns[i]->d = second_r2data->columns[i]->d * (1.0 -instead_of_pow((1.0 - (second_r2data->columns[curr_column]->d/second_old_d)),second_r2data->columns[i]->f/second_r2data->columns[i]->d));
      second_r2data->columns[i]->f = second_r2data->columns[curr_column]->f;
//        printf("(second) Thus, new d is %f\n",second_r2data->columns[i]->d );
    }
  }
//  printf("Statistics Updated! New stats:\n Low %ju \n Upper %ju \n Arithmos Stoixeion %f \n Arithmos Monadikon Stoixeion %f\n",r2data->columns[curr_column]->l, r2data->columns[curr_column]->u,r2data->columns[curr_column]->f,r2data->columns[curr_column]->d);
  return r2data->columns[curr_column]->f;
  //sto join metaksi 2 pinakwn, h prosvasi ston deutero pinaka na ginetai meso ths second_r2data. na kanw tis katalliles allages
}

batch *InitBatch(){
  batch *my_batch=malloc(sizeof(batch));
  my_batch->num_of_queries=0;
  my_batch->queries_table=malloc(sizeof(query*));
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
  if (my_batch->queries_table==NULL)
    printf("Error on realloc!\n");
  my_batch->queries_table[my_batch->num_of_queries] = (query *)malloc(sizeof(query));
  my_batch->queries_table[my_batch->num_of_queries]->sxeseis = tokens[0];
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
  if(pred->rel1==-1)
  {
    rel2 = relations[pred->rel2]->columns[pred->col2];
    if(inb_res->joined[pred->rel2]==-1){
      relation = rel2;
    }else{
      relation = BuildRelation(inb_res,pred->rel2,rel2);
    }
    result2 = InitInbetList();
    compute_operation(pred->op,pred->col1,relation,result2);
    inb_res = UpdateInbetList2(inb_res,result2,pred->rel2);
  }else if(pred->rel2==-1){
    rel1 = relations[pred->rel1]->columns[pred->col1];
    if(inb_res->joined[pred->rel1]==-1){
      relation = rel1;
    }else{
      relation = BuildRelation(inb_res,pred->rel1,rel1);
    }
    result1 = InitInbetList();
    compute_operation(pred->op,pred->col2,relation,result1);
    inb_res = UpdateInbetList2(inb_res,result1,pred->rel1);
  }else if(pred->rel1==pred->rel2){ //self join
/*    rel1 = relations[pred->rel1]->columns[pred->col1];
    if(inb_res->joined[pred->rel1]!=-1){
      rel1 = BuildRelation(inb_res,pred->rel1,rel1);
    }
    result1 = InitInbetList();
    rel2 = relations[pred->rel2]->columns[pred->col2];
    if(inb_res->joined[pred->rel2]!=-1){
      rel2 = BuildRelation(inb_res,pred->rel2,rel2);
    }
    result2 = InitInbetList();
*/

  }else{

    rel1 = relations[pred->rel1]->columns[pred->col1];
    rel2 = relations[pred->rel2]->columns[pred->col2];
    if(inb_res->joined[pred->rel2]!=-1){
      rel2 = BuildRelation(inb_res,pred->rel2,rel2);
    }
    if(inb_res->joined[pred->rel1]!=-1){
      rel1 = BuildRelation(inb_res,pred->rel1,rel1);
    }
    result1 = InitInbetList();
    result2 = InitInbetList();
    if(pred->op=='='){
      if(inb_res->joined_pairs[pred->rel1][pred->rel2]!=-1){
        SerialCompare(rel1,rel2,result1,result2);
      }else{
        RadixHashJoin(rel1,rel2,result1,result2);
      }
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
  int * order_of_joins = NULL;
  int * order_of_filters = NULL;
  int * curr_join = (int*)malloc(sizeof(int));
  int * curr_filter = (int*)malloc(sizeof(int));
  *curr_join=0;
  *curr_filter=0;
  int next_pred=-1;
  inbetween_results *inb_res=InitInbetResults(in_query->num_of_relations);
  for(int i=0;i<in_query->num_of_predicates;i++){
    next_pred = select_predicate(curr_join,curr_filter, in_query->num_of_predicates,in_query->katigorimata,relations,inb_res,order_of_joins,order_of_filters);
    printf("Next pred is %d\n",next_pred);
  //  printf("Now updating statistics of pred %d dld to pred %d.%d %c %d.%d...\n",next_pred,in_query->katigorimata[next_pred]->rel1,in_query->katigorimata[next_pred]->col1,in_query->katigorimata[next_pred]->op,in_query->katigorimata[next_pred]->rel2,in_query->katigorimata[next_pred]->col2);
    update_statistics(relations,in_query->katigorimata,next_pred);
    inb_res = execute_predicate(in_query->katigorimata[next_pred],relations,inb_res);
    in_query->katigorimata[next_pred]->op= '.';
  }
  for (int k=0;k<in_query->num_of_predicates;k++)
  {
  restore_statistics(relations, in_query->katigorimata[k]->rel1);
  if (in_query->katigorimata[k]->rel2!=-1)
    restore_statistics(relations, in_query->katigorimata[k]->rel2);
  }
  printf("-------------------\n");
  show_results(inb_res,relations,in_query->proboles);
  printf("\n-------------------\n");
  //free inb_res , relations
  free (curr_join);
  free (order_of_joins);
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
  build_num += compute_num[j-k]-'0';
  return build_num;
}

predicates **fill_predicates (char * token, int num_predicates)
{
  char flag;
  int i,j,k,num_of_relations_left,num_of_relations_right,num_length=0,build_num=0;
  char * compute_num,* query;
  predicates **pred = malloc(sizeof(predicates *)*num_predicates);


  query = strtok(token,"&");
  for (i=0;i<num_predicates;i++)
  {
    pred[i]=malloc(sizeof(predicates));
    num_of_relations_left=0;
    num_of_relations_right=0;
    num_length=0;
    flag=0;
    compute_num = query;
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
int search_table (int ** join_table,int size_of_table, int r, int c)
{
//  printf(">Checking given %d.%d< \n",r,c);
  int i;
  if (size_of_table==-1)
    return 1;
  for (i=0;i<=size_of_table;i++)
  {
  //  printf("Checking jt %d.%d with given %d.%d\n",join_table[i][0],join_table[i][1],r,c);
    if ((join_table[i][0]==r) && (join_table[i][1]==c))
      return 0;
  }
  //printf("den to brika\n");
  return 1;
}
int select_predicate(int * curr_join, int * curr_filter, int numofPredicates, predicates ** input_predicates,relation_data ** datatable, inbetween_results *res, int * order_of_joins, int * order_of_filters){
  /*
    epistrefei to predicate pou tha ektelestei. ekteloume prota ta filtra.
  */
  int num_of_join_rel,min,pos;
  int i,j,return_predicate=-1,weight,constant,distance_to_filter=0,rel,col,max,k;
  int ** filter_cost_table;
  int ** join_table;
  char filters_remaining=0;
  int executed_pred=0;
  int min_weight=-1;
  int num_of_filter_pred=0;
  int num_of_join_pred=0;
  int prev_rel;
  int prev_col;
  if (order_of_filters==NULL)
  {
    for (j=0;j<numofPredicates;j++)
    {
      if (input_predicates[j]->op!='.' && input_predicates[j]->rel2==-1)
      {
        num_of_filter_pred++;
      }
    }
    order_of_filters = (int*)malloc(sizeof(int)*num_of_filter_pred);
    filter_cost_table = (int**)malloc(sizeof(int*)*num_of_filter_pred);
    for (i=0;i<num_of_filter_pred;i++)
      {
        filter_cost_table[i] = malloc(sizeof(int)*2);
      }
    i=0;
        for (k=0;k<numofPredicates;k++)
        {
          if (input_predicates[k]->op!='.' && input_predicates[k]->rel2==-1)
          {
            filter_cost_table[i][0] = k;
            filter_cost_table[i][1] = update_statistics(datatable, input_predicates, k);
            i++;
          }
        }
        //restore the statistics
        for (k=0;k<numofPredicates;k++)
        {
          if (input_predicates[k]->op!='.' && input_predicates[k]->rel2==-1)
          {
            restore_statistics (datatable, input_predicates[k]->rel1); 
          }
        }
    for (int j=0; j<num_of_filter_pred;j++)
    {
      k=0;
      while (filter_cost_table[k][1]==-1)
        {
          k++;
        }
      pos =k ;
      min = filter_cost_table[k][1];
      for (i=0;i<num_of_filter_pred;i++)
        {
          if (filter_cost_table[i][1]<min)
          {
            filter_cost_table[i][1]=min;
            pos = i;
          }
        }
        order_of_filters[j] = filter_cost_table[pos][0];
        filter_cost_table[pos][1] = -1;
      }

  }
    for (j=0;j<numofPredicates;j++)
      {
        if (input_predicates[j]->op!='.' && input_predicates[j]->rel2==-1)
        {
          filters_remaining = 1;
          break;
        }
      }
    if (filters_remaining==1)
      {
//      printf("Ekteloume to panemorfo filtraki me pred num %d\n",order_of_filters[(*curr_filter)]);
      return order_of_filters[(*curr_filter)++];
      }
    else
    {
//printf("Telos me ta filtra. Pame sta joinakia mas\n");
//meta ta join
if (order_of_joins == NULL)
{
  for (int j=0;j<numofPredicates;j++)
  {
     if (input_predicates[j]->op=='.')
      executed_pred++;
  }
  num_of_join_pred = numofPredicates-executed_pred;
  if (num_of_join_pred==1)//an einai ena tsilare apla kane return
  {
    for (int j=0;j<numofPredicates;j++)
    {
      if (input_predicates[j]->op!='.')
      {
        return j;
      }
    }
  }
//  printf("num of join pred is %d because numofPredicates is %d and executed_pred is %d||",num_of_join_pred,numofPredicates,executed_pred);
  order_of_joins = (int*)malloc(sizeof(int)*(num_of_join_pred)); //we have these many joins
  join_table = (int**)malloc(sizeof(int*)*2*num_of_join_pred);
  i=0;
  j=-1;
  while (i<numofPredicates)
  {
//    printf("\nTore blepoume to predicate %d.%d = %d.%d\n",input_predicates[i]->rel1,input_predicates[i]->col1,input_predicates[i]->rel2,input_predicates[i]->col2);
    if ((input_predicates[i]->op!='=') || (input_predicates[i]->rel2==-1))
      {
        i++;
        continue;
      }
    j++;
    join_table[j] = malloc(2* sizeof(int));
    join_table[j][0]=input_predicates[i]->rel1;
    join_table[j][1]=input_predicates[i]->col1;
  //  printf("Empiksa to %d.%d\n",    join_table[j][0],    join_table[j][1]);
    j++;
    join_table[j] = malloc(2* sizeof(int));
    join_table[j][0]=input_predicates[i]->rel2;
    join_table[j][1]=input_predicates[i]->col2;
  //  printf("Empiksa to %d.%d\n",    join_table[j][0],    join_table[j][1]);
    i++;
  }
  num_of_join_rel = ++j;
//  printf("we have created the following join table with %d rels\n",num_of_join_rel);
//  for (j=0;j<num_of_join_rel;j++)
//  {
//    printf("[%d.%d]\n\n", join_table[j][0],join_table[j][1]);
//  }
  order_of_joins = find_permutations (num_of_join_rel, order_of_joins, datatable, input_predicates, numofPredicates, join_table);
}
  //printf("returning pred %d\n",order_of_joins[*curr_join]);
    return order_of_joins[*curr_join++];
  }
}

void FreeBatch(batch *b){
  if(b==NULL) return ;
  for(int k=0;k<b->num_of_queries;k++){
    free(b->queries_table[k]->sxeseis);
    free(b->queries_table[k]->proboles);
    for(int i=0;i<b->queries_table[k]->num_of_predicates;i++){
      free(b->queries_table[k]->katigorimata[i]);
    }
    free(b->queries_table[k]->katigorimata);
  }
  free(b);
}
