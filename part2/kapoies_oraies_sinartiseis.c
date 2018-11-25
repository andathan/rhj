#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "structs.h"
#include "rhj.h"


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

void seperate_predicate (char * input, char * tokens[3] )
{
  tokens[0] = strtok(input,"|");
  tokens[1] = strtok(NULL,"|");
  tokens[2] = strtok(NULL,"|");
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
int make_number (int num_length,char * compute_num, int j)
{
  int k;
  int build_num=0;
  for (k=num_length;k>1;k--)
  {
  build_num+= pow(10,(k-1))*(compute_num[j-k]-'0');
  }
  build_num+= compute_num[j-k]-'0';
  return build_num;
}

void  fill_predicates (char * token, int num_predicates, predicates ** predicates)
{
  int i,j,k;
  char flag;
  int num_of_relations_left;
  int num_of_relations_right;
  int num_length=0;
  int build_num=0;
  char * compute_num;
  char * query;
  query = strtok(token,"&");
  for (i=0;i<num_predicates;i++)
  {
    num_of_relations_left=0;
    num_of_relations_right=0;
    num_length=0;
    flag=0;
    compute_num = query;
    printf("Checking query %s\n",query);
    predicates[i] = malloc(sizeof(predicates));
    for(j=0;j<strlen(compute_num);j++)
    {
      if(compute_num[j]>='0' && compute_num[j]<='9')
        num_length++;
      if (compute_num[j] == '>' || compute_num[j] == '<' || compute_num[j] == '=')
      {
        build_num = make_number(num_length, compute_num, j);
        num_length=0;
        predicates[i]->col1= build_num;
        if (num_of_relations_left==0) //exoume apla metabliti aristera
        {
          predicates[i]->rel1=-1;
        }
        flag=1;
        predicates[i]->op = compute_num[j];
      }
      if(compute_num[j] == '.' && flag==0)
      {
        build_num = make_number(num_length, compute_num, j);
        num_length=0;
        if (num_of_relations_left==0)
          predicates[i]->rel1=build_num;
        else
          predicates[i]->col1=build_num;
        num_of_relations_left++;
        }
      else if (compute_num[j]=='.' && flag==1)
      {
        build_num = make_number(num_length, compute_num, j);
        num_length=0;
        if (num_of_relations_right==0)
          predicates[i]->rel2=build_num;
        else
          predicates[i]->col2=build_num;
        num_of_relations_right++;
      }
    }
    build_num = make_number(num_length, compute_num, j);
    num_length=0;
    predicates[i]->col2=build_num;
    if (num_of_relations_right==0)//exoume apla metabliti deksia
    {
      predicates[i]->rel2=-1;
    }
     query = strtok(NULL,"&");
  }
}


relation_data **find_corresponding(char * token,all_data *datatable)
{
  int j,build_num=0;
  int corresponding_counter=0;
  int num_length=0;
  int num_relations=1;
  relation_data **corresponding_table;
    //first find how many relations we have
  for (j=0;j<=strlen(token);j++)
  {
    if (isspace(token[j]))
    {
      num_relations++;
    }
  }
  corresponding_table =(relation_data **)malloc(sizeof(relation_data *)*num_relations);
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

void compute_operation(char op, int constant,Relation * relation,inbet_list * A)
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
      UpdateInbetList (A, relation->tuples->key);
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

void show_proboles(inbetween_results *inb_results,all_data *input_data, char * token) //thelei ligi doulitsa
{
  int j,build_num,k;
  int table;
  int column;
  int num_length=0;
  uint64_t sum=0;
  char flag=0;
  //first find table.column requested
  for (j=0;j<=strlen(token);j++)
  {
    if(token[j]>='0' && token[j]<='9')
    {
      num_length++;
    }
    else if (token[j]=='.' && flag == 0)
    {
      build_num = make_number(num_length, token, j);
      num_length=0;
      table = build_num;
      flag = 1;
    }
    else if ((token[j]==' ' || token[j] == '\0') && flag == 1)
    {
      build_num = make_number(num_length, token, j);
      num_length=0;
      column = build_num;
      flag = 0;
      printf("Showing %d.%d\n",table,column);
    }
    else{
      printf("Error! Char %c appeared\n",token[j]);
    }
    if(inb_results->inbet_lists[table] == NULL)
      printf("Table.Column %d.%d doesnt have any results.\n",table,column);
      //to bala se sxolia gia to compile
/*    else
    {
      for (k=0;k<result_relation->numTuples;k++)
      {
        sum += inb_results->inbet_list->inbetween_lists[table]->tuples->payload;
      }
      printf ("Table.Column %d.%d  sum is %d\n");
    }
  }*/
  }
}

int main (void)
{
  inbetween_results *inb_results = InitInbetResults(1);
  Relation * relation,* relation2;
  all_data * datatable;
  char * tokens[3];
  int i;
  char input[80] = "0 2 4|0.1=1.2&1.0=2.1&2.9>1821|0.0 1.1";

  seperate_predicate(input,tokens);
  printf("1:%s\n2:%s\n3:%s\n",tokens[0],tokens[1],tokens[2]);
  relation_data ** arrays_in_query= find_corresponding(tokens[0],datatable);
  int num_predicates = find_num_of_predicates(tokens[1]);
  printf("We have %d predicate\n",num_predicates);
  predicates ** predicates_table = malloc(sizeof(predicates *) * num_predicates);
  fill_predicates(tokens[1],num_predicates,predicates_table);
  print_predicates(predicates_table,num_predicates);
  //apo edw kai katw untested kodikas
  for(i=0;i<num_predicates;i++)
  {
    if(predicates_table[i]->rel1==-1) //einai tis morfis constant op relation column
    {
      if(inb_results->inbet_lists[predicates_table[i]->rel1] != NULL )
      {
        relation = BuildRelation(inb_results->inbet_lists[predicates_table[i]->rel1],arrays_in_query[predicates_table[i]->rel1]->columns[predicates_table[i]->col1]);
        ReInitInbetList(inb_results->inbet_lists[predicates_table[i]->rel1]);
        // free inbet_list->inbetween_lists[predicates_table[i]->rel1]
        //init
      }
      else
      {
        relation = arrays_in_query[predicates_table[i]->rel1]->columns[predicates_table[i]->col1];
      }
      compute_operation(predicates_table[i]->op,predicates_table[i]->col1, relation,inb_results->inbet_lists[predicates_table[i]->rel1]);
    }
    else if(predicates_table[i]->rel2==-1) //einai tis morfis relation column op constant
    {
      if(inb_results->inbet_lists[predicates_table[i]->rel1] != NULL )
      {
        relation = BuildRelation(inb_results->inbet_lists[predicates_table[i]->rel2],arrays_in_query[predicates_table[i]->rel2]->columns[predicates_table[i]->col2]);
        ReInitInbetList(inb_results->inbet_lists[predicates_table[i]->rel2]);
      }
      else
      {
        relation = arrays_in_query[predicates_table[i]->rel2]->columns[predicates_table[i]->col2];
      }
      compute_operation(predicates_table[i]->op,predicates_table[i]->col1, relation,inb_results->inbet_lists[predicates_table[i]->rel2]);
    }
    else if (predicates_table[i]->rel1 == predicates_table[i]->rel2) //join sto idio relation, diladi sarosi (mporei na einai join se alla columns)
    {
      if(inb_results->inbet_lists[predicates_table[i]->rel1] != NULL )
      {
        relation = BuildRelation(inb_results->inbet_lists[predicates_table[i]->rel1],arrays_in_query[predicates_table[i]->rel1]->columns[predicates_table[i]->col1]);
        ReInitInbetList(inb_results->inbet_lists[predicates_table[i]->rel1]);
      }
      else
      {
        relation = arrays_in_query[predicates_table[i]->rel1]->columns[predicates_table[i]->col1];
      }
      if(inb_results->inbet_lists[predicates_table[i]->rel1] != NULL )
      {
        relation2 = BuildRelation(inb_results->inbet_lists[predicates_table[i]->rel2],arrays_in_query[predicates_table[i]->rel2]->columns[predicates_table[i]->col2]);
        ReInitInbetList(inb_results->inbet_lists[predicates_table[i]->rel2]);

      }
      else
      {
        relation2 = arrays_in_query[predicates_table[i]->rel2]->columns[predicates_table[i]->col2];
      }
      //we have built the relations. now compute
      for(int j=0;j<relation->num_tuples;j++)
      {
        compute_operation(predicates_table[i]->op,relation->tuples[j].payload,relation2,inb_results->inbet_lists[predicates_table[i]->rel1]);
      }
    }
    else //exw join
    {
      if(inb_results->inbet_lists[predicates_table[i]->rel1] != NULL )
      {
        relation = BuildRelation(inb_results->inbet_lists[predicates_table[i]->rel1],arrays_in_query[predicates_table[i]->rel1]->columns[predicates_table[i]->col1]);
        ReInitInbetList(inb_results->inbet_lists[predicates_table[i]->rel1]);

      }
      else
      {
        relation = arrays_in_query[predicates_table[i]->rel1]->columns[predicates_table[i]->col1];
      }
      if(inb_results->inbet_lists[predicates_table[i]->rel1] != NULL )
      {
        relation2 = BuildRelation(inb_results->inbet_lists[predicates_table[i]->rel2],arrays_in_query[predicates_table[i]->rel2]->columns[predicates_table[i]->col2]);
        ReInitInbetList(inb_results->inbet_lists[predicates_table[i]->rel2]);
      }
      else
      {
        relation2 = arrays_in_query[predicates_table[i]->rel2]->columns[predicates_table[i]->col2];
      }
      //we have built the relations. now RHJ
      RadixHashJoin(relation,relation2,inb_results->inbet_lists[predicates_table[i]->rel1],inb_results->inbet_lists[predicates_table[i]->rel2]);
    }
  }
//  show_proboles(inb_results,all_data,tokens[2]); //thelei ligi doulitsa.
  free (arrays_in_query); //to coressponding table xriazete?
}
