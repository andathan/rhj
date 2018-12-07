#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "predicates.h"

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
