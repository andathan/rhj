#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "predicates.h"
#include "trees.h"


predicate * make_predicate (int rel1, int col1, int rel2, int col2, char op)
{
  predicate * new_pred = malloc(sizeof(predicate));
  new_pred->rel1 = rel1;
  new_pred->col1= col1;
  new_pred->op = op;
  new_pred->rel2=rel2;
  new_pred->col2=col2;
}
void restore_statistics (relation_data ** relations, int rel)
{
  int i;
  relation_data * r2data;
  if (rel1!=-1)
    r2data = relations[rel1];

  for (k=0;k<r2data->numColumns;k++)
  {
    r2data->columns[k]->l = r2data->columns[k]->prev_l;
    r2data->columns[k]->u = r2data->columns[k]->prev_u;
    r2data->columns[k]->f = r2data->columns[k]->prev_f;
    r2data->columns[k]->d = r2data->columns[k]->prev_d;
    r2data->columns[k]->restored=0;
  }

}
float update_statistics(relation_data ** relations, predicates * curr_pred){
  int curr_column = curr_pred->col1,k;
  float old_d,second_old_d;
  relation_data * r2data, * second_r2data;
  if (curr_pred->rel1!=-1)
    r2data = relations[curr_pred->rel1];
  if (curr_pred->rel2!=-1)
    {
    second_r2data = relations[curr_pred->rel2];
    printf("d is %f\n",second_r2data->columns[curr_pred->col2]->d);
    }
  else
    second_r2data = NULL;
  int i = curr_column;
  float n = (float)(r2data->columns[i]->u - r2data->columns[i]->l +1);
  float old_f= r2data->columns[i]->f;//used for storing previous f value
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
        printf("%d exists in d_table\n",curr_pred->col2);
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
          printf("%d exists in d_table\n",curr_pred->col2);
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
        r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -pow((1.0- (r2data->columns[curr_column]->f/old_f)),r2data->columns[i]->f/r2data->columns[i]->d));
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
      r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -pow((1.0- (r2data->columns[curr_column]->f/old_f)),r2data->columns[i]->f/r2data->columns[i]->d));
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
      r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -pow((1.0- (r2data->columns[curr_column]->f/old_f)),r2data->columns[i]->f/r2data->columns[i]->d));
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
    r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -pow((1.0- (r2data->columns[curr_column]->f/old_f)),(r2data->columns[i]->f/r2data->columns[i]->d)));
    r2data->columns[curr_pred->col2]->d = r2data->columns[i]->d;
    for (i=0;i<r2data->numColumns;i++)
    {
      if (i==curr_column)
        continue;
      r2data->columns[i]->d = r2data->columns[i]->d * (1.0 -pow((1.0- (r2data->columns[curr_column]->f/old_f)),r2data->columns[i]->f/r2data->columns[i]->d));
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
    //first keep old values in case we need to restore them
    //for table A
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
      printf("\nd of i column is %f| d of curr_column %f| old_d is %f| f of i columns is %f | d of i column is %f\n", r2data->columns[i]->d , r2data->columns[curr_column]->d,old_d,r2data->columns[i]->f,r2data->columns[i]->d);
      r2data->columns[i]->d = r2data->columns[i]->d * (1.0 - pow((1.0 - (r2data->columns[curr_column]->d/old_d)),r2data->columns[i]->f/r2data->columns[i]->d));
      printf("Thus, new d is %f\n",r2data->columns[i]->d );
      r2data->columns[i]->f = r2data->columns[curr_column]->f;
    }
    //for table B
    for (i=0;i<second_r2data->numColumns;i++)
    {
      if (i==curr_pred->col2)
        continue;
      printf("(second) d of i column is %f| d of curr_column %f| old_d is %f| f of i columns is %f | d of i column is %f, pow is %f\n", second_r2data->columns[i]->d,second_r2data->columns[curr_column]->d,second_old_d,second_r2data->columns[i]->f,second_r2data->columns[i]->d,pow((1.0 - (second_r2data->columns[curr_column]->d/second_old_d)),second_r2data->columns[i]->f/second_r2data->columns[i]->d));
      second_r2data->columns[i]->d = second_r2data->columns[i]->d * (1.0 -pow((1.0 - (second_r2data->columns[curr_column]->d/second_old_d)),second_r2data->columns[i]->f/second_r2data->columns[i]->d));
      second_r2data->columns[i]->f = second_r2data->columns[curr_column]->f;
        printf("(second) Thus, new d is %f\n",second_r2data->columns[i]->d );
    }
  }
  printf("Statistics Updated! New stats:\n Low %ju \n Upper %ju \n Arithmos Stoixeion %f \n Arithmos Monadikon Stoixeion %f\n",r2data->columns[curr_column]->l, r2data->columns[curr_column]->u,r2data->columns[curr_column]->f,r2data->columns[curr_column]->d);
  return r2data->columns[i]->f;
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
  int next_pred=-1;
  inbetween_results *inb_res=InitInbetResults(in_query->num_of_relations);
  for(int i=0;i<in_query->num_of_predicates;i++){
    next_pred = select_predicate(in_query->num_of_predicates,in_query->katigorimata,relations,next_pred,inb_res);
    printf("Next pred is %d\n",next_pred);
    printf("Now updating statistics...\n");
    update_statistics(relations,in_query->katigorimata[next_pred]);
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

int select_predicate(int numofPredicates, predicates ** input_predicates,relation_data ** datatable, int previous_predicate,inbetween_results *res){
//girnaei ton arithmo me to predicate pou prepei na ektelesoume prota. theloume prota auta pou tha bgoun grigora
  /*
    epistrefei to predicate pou tha ektelestei. ekteloume prota ta filtra.
    apo ta filtra dinoume perissotero baros se auta pou exoun megalo ginomeno diaspora * apostasi teleutaias timis apo to filtro.
     meta ta joins
  */
  int i,return_predicate=-1,weight,constant,distance_to_filter=0,rel,col,max,min;
  int best=0;
  int min_weight=-1;
  char no_more_filters=1;
  char select_filters = 1;
  int prev_rel;
  int prev_col;

  for (int j=0;j<numofPredicates;j++){
    if (input_predicates[j]->op!='.'){
      return j;
    }
  }
/*
  for (int j=0;j<numofPredicates;j++){
    if ((input_predicates[j]->rel1 == -1 || input_predicates[j]->rel2 == -1) && input_predicates[j]->op!='.'){
      no_more_filters = 0;
      break;
    }
  }
  if (no_more_filters==1)//teleiosame me ta filters, pame sta joins
    select_filters = 0;
  for(i=0;i<numofPredicates;i++){
    if(previous_predicate!=-1 && res->joined[input_predicates[i]->rel1]==-1 && res->joined[input_predicates[i]->rel2]==-1 ){
        continue;
    }
    if(select_filters==1){
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
      if((input_predicates[i]->rel1 != -1 &&  input_predicates[i]->rel2 != -1) && input_predicates[i]->op != '.' ){ // join pou den exei ginei
            return_predicate = i;
      }
    }
  }
  */
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
