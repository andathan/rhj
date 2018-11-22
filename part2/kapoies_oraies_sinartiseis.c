#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct predicate_type{
  int rel1;
  int col1;
  int rel2;
  int col2;
  char op;
}predicate_type;

void print_predicates (predicate_type ** predicates,int num_predicates)
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

int make_number  (int num_length,char * compute_num, int j)
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

void  fill_predicate_types (char * token, int num_predicates, predicate_type ** predicates)
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
    predicates[i] = malloc(sizeof(predicate_type));
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

void call_op (char op, int constant; )
{


}

int main (void)
{
  char * tokens[3];
  char input[180] = "0 2 4|0.1=1.2&1.0=2.1|0.0 1.1";
  seperate_predicate(input,tokens);
  printf("1:%s\n2:%s\n3:%s\n",tokens[0],tokens[1],tokens[2]);
  int num_predicates = find_num_of_predicates(tokens[1]);
  printf("We have %d predicate\n",num_predicates);
  predicate_type ** predicates = malloc(sizeof(predicate_type *)*num_predicates);
  fill_predicate_types(tokens[1],num_predicates,predicates);
  print_predicates(predicates,num_predicates);
  for(i=0;i<num_predicates;i++)
  {
    if (!strcmp(predicates[i])->op,"=")
    {
        if(predicates[i]->rel1 == -1 || predicates[i]->rel2 == -1) //den exw join
        {
            if(predicates[i]->rel1==-1) //einai tis morfis constant op relation column
            {
              if(stepping_stone->stepping_arrays[predicates[i]->rel1] != NULL )
              {
                relation = build_relation(stepping_stone->stepping_arrays[predicates[i]->rel1],arrays_in_query[predicates[i]->rel1]->columns[col1])
                constant = col2
              }
              else
              {
                relation = arrays_in_query[predicates[i]->rel1]->columns[col1]
                constant=col2 // o arithmos pou tha sigkrinw
              }
            }
    }

  }
}
