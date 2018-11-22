#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#define BUFFERSIZE 100


struct tuple{
  int32_t key;    //rowID
  int32_t payload;   //value
};
typedef struct tuple tuple;

struct Relation{
  tuple  *tuples;   //pinakas apo tuples
  uint32_t num_tuples;  //megethos pinaka
};
typedef struct Relation Relation;

struct relation_data{
  uint64_t numColumns;
  uint64_t numTuples;
  Relation **columns;     //pinakas apo deiktes stin arxi kathe column
};
typedef struct relation_data relation_data;

struct all_data{
  relation_data ** table;  //array of pointers to relation_data
  int num_relations;       //number of total relations
};
typedef struct all_data all_data;

struct predicates{      /* struct predicates stores info for predicate in query*/
  int rel1;             /* index of 1st relation in predicate*/
  int col1;             /* index of column of 1st relation in predicate*/
  int rel2;             /* index of 2nd relation in predicate*/
  int col2;             /* index of column of 2nd relation in predicate*/
  char op[3];           /* operation between the 2 columns */
};

typedef struct predicates predicates;

struct inbet_node{           /*struct to store inbetween relations*/
  int rowIDS[BUFFERSIZE];        /*array of rowIDS*/
  int num_tuples;               /*number of tuples in array*/
  struct inbet_node *next;          /*pointer to the next node */
};
typedef struct inbet_node inbet_node;

struct inbet_list{
  inbet_node *head;       /*pointer to the head of the list*/
  inbet_node *current;    /*pointer to the current node of the list*/
  int total_tuples;       /*total tuples in list*/
};
typedef struct inbet_list inbet_list;

struct inbetween_results{
  inbet_list **inbet_lists; /*array of pointers to lists*/
  int num_lists;                  /*number of lists*/
};
typedef struct inbetween_results inbetween_results;


void print_predicates (predicates ** predicates,int num_predicates)
{
  int i;
  for (i=0;i<num_predicates;i++)
    {
      if (predicates[i]->rel1!=-1 && predicates[i]->rel2!=-1)
      printf("%d.%d%c%d.%d\n",predicates[i]->rel1,predicates[i]->col1,predicates[i]->op[0],predicates[i]->rel2,predicates[i]->col2);
      else if (predicates[i]->rel1==-1)
      printf("%d%c%d.%d\n",predicates[i]->col1,predicates[i]->op[0],predicates[i]->rel2,predicates[i]->col2);
      else if (predicates[i]->rel2==-1)
      printf("%d.%d%c%d\n",predicates[i]->rel1,predicates[i]->col1,predicates[i]->op[0],predicates[i]->col2);
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
        predicates[i]->op[0] = compute_num[j];
        predicates[i]->op[1] = 0;
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


int *  find_corresponding(char * token)
{
  int j,build_num=0;
  int corresponding_counter=0;
  int num_length=0;
  int num_relations=1;
  int * corresponding_table;
    //first find how many relations we have
    for (j=0;j<=strlen(token);j++)
    {
      if (isspace(token[j]))
      {
        num_relations++;
      }
    }
    corresponding_table = malloc(sizeof(int) * num_relations);
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
      corresponding_table[corresponding_counter] = build_num;
      printf("%d corresponds to %d\n", corresponding_table[corresponding_counter],corresponding_counter);
      corresponding_counter++;
    }
  }
  return corresponding_table;
}

void compute_operation (char op, int constant,Relation * relation)
{
  int i;
  if (op == '=')
  {
    for (i=0;i<relation->num_tuples;i++)
    {
      if (relation->tuples->payload == constant)
      {
        //insert sta results relation->tuples->key
      }
    }
  }
  else if (op == '>')
  {
    for (i=0;i<relation->num_tuples;i++)
    {
      if (relation->tuples->payload > constant)
      {
        //insert sta results relation->tuples->key
      }
    }
  }
  else if (op =='<')
  {
    for (i=0;i<relation->num_tuples;i++)
    {
      if (relation->tuples->payload < constant)
      {
        //insert sta results relation->tuples->key
      }
    }
  }
}

void show_proboles (inbetween_results inb_results, all_data input_data)
{
  Relation * result_relation; //thelei malloc? afou kanoume malloc stin build relation
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
    else
      printf("Error! Char %c appeared\n",token[j]);
  // result_relation =  build_relation apo inb_results
  if (result_relation == NULL)
    printf("Table.Column %d.%d doesnt have any results.\n",table,column);
  else
    {
      for (k=0;k<result_relation->numTuples;k++)
      {
        sum += result_relation->tuples->payload;
      }
      printf ("Table.Column %d.%d  sum is %d\n");
    }
  }
}
int main (void)
{
  inbetween_results *inb_results = InitInbetResults (1);
  char * tokens[3];
  int i;
  char input[80] = "0 2 4|0.1=1.2&1.0=2.1&2.9>1821|0.0 1.1";
  seperate_predicate(input,tokens);
  printf("1:%s\n2:%s\n3:%s\n",tokens[0],tokens[1],tokens[2]);
  int * corresponding_table= find_corresponding(tokens[0]);
  int num_predicates = find_num_of_predicates(tokens[1]);
  printf("We have %d predicate\n",num_predicates);
  predicates ** predicates_table = malloc(sizeof(predicates *) * num_predicates);
  fill_predicates(tokens[1],num_predicates,predicates_table);
  print_predicates(predicates_table,num_predicates);
  find_provoles(tokens[2]);
//apo edw kai katw untested kodikas
  for(i=0;i<num_predicates;i++)
  {
        if(predicates[i]->rel1==-1) //einai tis morfis constant op relation column
            {
            if(inbet_list->inbetween_lists[predictes[i]->rel1] != NULL )
              {
              //  relation = build_relation(inbet_list->inbetween_lists[predictes[i]->rel1],arrays_in_query[predicates[i]->rel1]->columns[col1]);
              }
            else
              {
              //relation = arrays_in_query[predicates[i]->rel1]->columns[col1];
              }
              compute_operation(predicates[i]->op,predicates[i]->col1, relation);
            }
          else if(predicates[i]->rel2==-1) //einai tis morfis relation column op constant
          {
            if(inbet_list->inbetween_lists[predictes[i]->rel1] != NULL )
                {
                //  relation = build_relation(inbet_list->inbetween_lists[predictes[i]->rel1],arrays_in_query[predicates[i]->rel1]->columns[col1]);
                }
              else
              {
                  //relation = arrays_in_query[predicates[i]->rel1]->columns[col1];
              }
            compute_operation(predicates[i]->op,predicates[i]->col1, relation);
          }
          else if (predicates[i]->rel1 == predicates[i]->rel2) //join sto idio relation, diladi sarosi (mporei na einai join se alla columns)
          {
              //build relation 1
              //build relation 2
              if (relation1->num_tuples > relation2->num_tuples) //??? exei noima auto?
                {
                  for (j=0;j<relation1->num_tuples;j++)
                  {
                    compute_operation(predicates[i]->op,relation1->tuples[j]->payload,relation2)
                  }
                }
              else                                            //??? exei noima auto?
                {
                  for (j=0;j<relation2->num_tuples;j++)
                  {
                    compute_operation(predicates[i]->op,relation2->tuples[j]->payload,relation1)
                  }
                }
          }
          else //exw join
          {
            //build relation 1
            //build relation 2
            RadixHashJoin (relation1,relation2);
            //insert results
          }
    }
  free (corresponding_table);
}
