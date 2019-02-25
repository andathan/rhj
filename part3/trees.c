#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "trees.h"
#include "rhj.h"
#include "predicates.h"
#include <stdio.h>


int find_pred_num(predicates ** predicate ,int num_predicates,int r1, int c1, int r2, int c2, char op)
{
  int i;
  for (i=0;i<num_predicates;i++)
  {
    if ((predicate[i]->rel1==r1 && predicate[i]->col1==c1 && predicate[i]->rel2==r2 && predicate[i]->col2==c2 && predicate[i]->op==op)
    ||  (predicate[i]->rel2==r1 && predicate[i]->col2==c1 && predicate[i]->rel1==r2 && predicate[i]->col1==c2 && predicate[i]->op==op))//h anapoda
      {
        return i;
      }
  }
  return -1;
}

tree * new(relation_data ** relations, int r1, int c1, int r2, int c2,predicates ** predicate, int pred_num){

  tree * leaf1 = (tree*)malloc(sizeof(tree));
  leaf1->rel = r1;
  leaf1->col = c1;
  leaf1->l = NULL;
  leaf1->r = NULL;
  leaf1->pred_num = -1;

  tree * leaf2 =  (tree*)malloc(sizeof(tree));
  leaf2->rel = r2;
  leaf2->col = c2;
  leaf2->l = NULL;
  leaf2->r = NULL;
  leaf2->pred_num = -1;

  tree * join =  (tree*)malloc(sizeof(tree));
  join->rel = -1;//join
  join->col = update_statistics(relations,predicate, pred_num);//find cost
  join ->l = leaf1;
  join ->r = leaf2;
  join->pred_num = pred_num;
  return join;
}
tree * insert ( relation_data ** relations, tree * root, int r, int c, predicates ** predicate, int pred_num)
{
  if (root->l->pred_num==pred_num)
    {
      return root;
    }
  tree * leaf = (tree*) malloc(sizeof(tree));
  leaf->rel = r;
  leaf->col = c;
  leaf->l = NULL;
  leaf->r = NULL;
  leaf->pred_num = -1;
  tree * join = (tree*) malloc(sizeof(tree));
  //printf("ftiaksame ena oreotato predicate %d.%d %c %d.%d",curr_pred->rel1,curr_pred->col1,curr_pred->op,curr_pred->rel2,curr_pred->col2);
  join->col = update_statistics(relations,predicate, pred_num);//find cost
  join->rel = -1;//join
  join ->l = root;
  join ->r = leaf;
  join->pred_num = pred_num;
  return join;
}
void restore_data(tree * root, relation_data ** relations)
{
    tree * view_leaf; //=  malloc(sizeof(tree));
    view_leaf = root;
    restore_statistics (relations, view_leaf->r->rel);
    view_leaf = view_leaf->l;
    while (1)
    {
    restore_statistics (relations, view_leaf->r->rel);
    if (view_leaf->l->rel!=-1)
      {restore_statistics (relations, view_leaf->l->rel);
       break;
     }
     else
      view_leaf= view_leaf->l;
    }
}
int calculate_total_cost(tree * root)
{
  if (root==NULL)
    printf("Tree is empty\n\n");
  int sum=0;
  tree * view_leaf; //=  malloc(sizeof(tree));
  view_leaf = root;
  if (view_leaf->col!=-1)
  sum+=view_leaf->col;
  while (1)
  {
    if (view_leaf->l!=NULL)
    {
    view_leaf= view_leaf->l;
    }
    else
      break;
    if (view_leaf->l!=NULL)
    {
    view_leaf= view_leaf->l;
    }
    else
      break;
    if (view_leaf->col!=-1)
    sum += view_leaf->col;
  }
//  printf("total cost is %d",sum);
  return sum;
}

//execute
int execute (tree * root, predicates ** predicate, int numofPredicates)
{
  int i,pred_num_to_return;
  char end_flag=0;
  tree * view_leaf; //=  (tree*)malloc(sizeof(tree));
  tree * trans_leaf; //= (tree*)malloc(sizeof(tree));
  tree * grand_dad; //= (tree*)malloc(sizeof(tree));
  trans_leaf = root;
  grand_dad= root;
  view_leaf = trans_leaf;
//  printf("EXC!\n");
  if (view_leaf==NULL)
    printf("view_leaf is NULl! Possible Errors!\n");
  if (view_leaf->l==NULL)//last call
  {
  pred_num_to_return = view_leaf->pred_num;
  if (view_leaf->l!=NULL)
//    free(view_leaf->l);
//  free(view_leaf->r);
//  free(view_leaf);
  return -1;
  }
else
{
  while((trans_leaf->l!=NULL || trans_leaf->r!=NULL) && trans_leaf!=NULL)
  {
    view_leaf = trans_leaf;
    trans_leaf = trans_leaf->l;
  }
  pred_num_to_return = view_leaf->pred_num;
    while (grand_dad->l!=view_leaf)
      {
        grand_dad=grand_dad->l;
        if (grand_dad==NULL)
          break;
      }
    if (grand_dad!=NULL)
    {
    if (view_leaf->l!=NULL)
      free (view_leaf->l);
    free (view_leaf->r);
    free(view_leaf);
    grand_dad->l=NULL;
    grand_dad->r=NULL;
  }
  }
  return pred_num_to_return;
}


int connected(predicates ** predicate, int num_predicates, int rel1, int col1, int rel2, int col2)
{
  int i;
//  printf("we are trying to find %d.%d = %d.%d|",rel1,col1,rel2,col2);
  for (i=0;i<num_predicates;i++)
  {
  // printf("[Now checking predicate %d.%d %c %d.%d]\n",predicate[i]->rel1,predicate[i]->col1,predicate[i]->op, predicate[i]->rel2,predicate[i]->col2);
    if ((predicate[i]->rel1 == rel1) && (predicate[i]->col1==col1) && (predicate[i]->op=='=') && (predicate[i]->rel2 == rel2) && (predicate[i]->col2==col2)
    || (predicate[i]->rel1 == rel2) && (predicate[i]->col1==col2) && (predicate[i]->op=='=') && (predicate[i]->rel2 == rel1) && (predicate[i]->col2==col1))//h anapoda
      {
  //    printf("connected\n");
        return 1;
      }
  }
  return 0;
}
void permutation_to_trees(exec_information ** store_information, int * store_info_counter, relation_data ** relations, predicates ** predicate, int num_predicates, int ** arr, int size)
{
    int i,j, pred_num;
    char flag=1;
//    for(i=0; i<size; i++)
//    {
//       printf("{%d.%d}\t",arr[i][0], arr[i][1]);
//    }
//    printf("\n\n");
    for(i=0; i<size; i=i+2)
    {
        if (i!=size-1)
        if (connected(predicate, num_predicates,arr[i][0], arr[i][1], arr[i+1][0], arr[i+1][1])==0)
          {
            flag=0;
            return;
          }
    }

//edw tha dimourgisei to tree based on pinaka arr
    tree * cost_tree = malloc(sizeof(tree));
    pred_num = find_pred_num(predicate,num_predicates, arr[0][0], arr[0][1], arr[1][0], arr[1][1],'=');
    cost_tree = new(relations, arr[0][0], arr[0][1], arr[1][0], arr[1][1],predicate, pred_num);

    for(i=2; i<size-1; i=i+2)
    {
      pred_num = find_pred_num(predicate,num_predicates,arr[i][0], arr[i][1], arr[i+1][0], arr[i+1][1],'=');
      cost_tree = insert (relations, cost_tree,arr[i][0],arr[i][1],predicate, pred_num);
    }
    store_information[*store_info_counter]->exec_tree=cost_tree;
    store_information[*store_info_counter]->total_cost=calculate_total_cost(cost_tree);
    (*store_info_counter)++;
    restore_data(cost_tree,relations);
}
void swap(int ** arr, int a, int b)
{
    int rel_temp= arr[a][0];
    int col_temp= arr[a][1];
    arr[a][0] = arr[b][0];
    arr[a][1] = arr[b][1];
    arr[b][0] = rel_temp;
    arr[b][1] = col_temp;
}
//permutation function
void permutation(exec_information ** store_information, int * store_info_counter, relation_data ** relations,predicates ** predicate, int num_predicates, int ** arr, int start, int end)
{
    int i;
    if(start==end)
    {
        permutation_to_trees(store_information,store_info_counter, relations, predicate, num_predicates, arr, end+1);
        return;
    }
    for(i=start;i<=end;i++)
    {
        swap(arr, i,start);
        permutation(store_information, store_info_counter, relations, predicate,num_predicates,arr, start+1, end);
        swap(arr, i, start);
    }
}
int  * find_permutations (int num_of_join_pred, int * order_of_joins, relation_data ** relations, predicates ** predicate, int num_predicates, int ** join_table)
{
  int i;
  int x;
  int next_pred;
  char finish=0;
  exec_information ** store_information;
  x = num_of_join_pred;
  for (i=x-1;i>0;i--)
    {
    x = x * i;//we wiil make x trees
  }
  store_information = (exec_information**)malloc(x*sizeof(exec_information*));
  for (i=0;i<x;i++)
    {
     store_information[i] = (exec_information*)malloc(sizeof(exec_information));
     store_information[i]->exec_tree = (tree*)malloc(sizeof(tree));
     store_information[i]->total_cost = -1;
    }
    int * store_info_counter=malloc(sizeof(int));
    *store_info_counter=0;
    permutation(store_information, store_info_counter, relations, predicate,  num_predicates, join_table, 0, num_of_join_pred-1);
    int selected_tree = find_min_cost(store_information,*store_info_counter);
      for (i=0;i<num_of_join_pred/2;i++)
      {
          next_pred = execute (store_information[selected_tree]->exec_tree, predicate, num_predicates);
           if (next_pred!=-1)
           {
           order_of_joins[i] = next_pred;
            }
           else
           {
             break;
           }
    }
    for (i=0;i<x;i++)
      {
        free(store_information[i]->exec_tree);
       free (store_information[i]);
    }
    free(store_info_counter);
    return order_of_joins;
}

int find_min_cost (exec_information ** store_information, int store_info_counter)
{
  int min,i;
  int k=0;
  while (store_information[k]->total_cost<0 && k<store_info_counter)
      k++;
  min = store_information[k]->total_cost;
  int pos =k;
  for (i=0;i<store_info_counter;i++)
  {
    if (store_information[i]->total_cost<min && store_information[i]->total_cost >-1)
      {min = store_information[i]->total_cost;
        pos = i;
      }
  }
  return pos;
}
