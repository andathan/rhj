#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "trees.h"
#include "predicates.h"
#include <stdio.h>
tree * new(relation_data ** relations, int r1, int c1, int r2, int c2){

  tree * leaf1 = malloc(sizeof(tree));
  leaf1->rel = r1;
  leaf1->col = c1;
  leaf1->l = NULL;
  leaf1->r = NULL;

  tree * leaf2 = malloc(sizeof(tree));
  leaf2->rel = r2;
  leaf2->col = c2;
  leaf2->l = NULL;
  leaf2->r = NULL;

  tree * join = malloc(sizeof(tree));
  join->rel = -1;//join
  predicates * curr_pred = malloc(sizeof(predicates));
  curr_pred =make_predicate(r1,c1,r2,c2,'=');
  join->col = update_statistics(relations,curr_pred);//find cost
  join ->l = leaf1;
  join ->r = leaf2;
  return join;
}
tree * insert ( relation_data ** relations, tree * root, int r, int c,)
{
  tree * leaf = malloc(sizeof(tree));
  tree * view_leaf =  malloc(sizeof(tree));
  leaf->rel = r;
  leaf->col = c;
  leaf->l = NULL;
  leaf->r = NULL;
  tree * join = malloc(sizeof(tree));
  join->rel = -1;//join
  view_leaf = root;
  while (view_leaf==-1)
  {
    view_leaf = view_leaf->l;
  }
  predicates * curr_pred =make_predicate(view_leaf->r->rel,view_leaf->->r->col,r,c,'=');
  join->col = update_statistics(relations,curr_pred);//find cost
  join->rel = -1;//join
  join ->l = root;
  join ->r = leaf;
  return join;
}
void restore_data(tree * root, relation_data ** relations)
{
    tree * view_leaf =  malloc(sizeof(tree));
    view_leaf = root;
    restore_statistics (relations, view_leaf->r);
    view_leaf = view_leaf->l;
    while (1)
    {
    restore_statistics (relations, view_leaf->r);
    if (view_leaf->l->rel!=-1)
      {restore_statistics (relations, view_leaf->l);
       break;
     }
     else
      view_leaf= view_leaf->l;
}

int calculate_total_cost(tree * root)
{
  int sum=0;
  tree * view_leaf =  malloc(sizeof(tree));
  view_leaf = root;
  sum+=view_leaf->col;
  while (1)
  {
    if (view_leaf!=NULL)
    {
    view_leaf= view_leaf->l;
    }
    else
      break;
    if (view_leaf!=NULL)
    {
    view_leaf= view_leaf->l;
    }
    else
      break;
    sum+= view_leaf->col;
  }
  printf("total cost is %d",sum);
  return sum;


//execute
char execute (tree * root, int * r1, int * r2, int * c1, int * c2)
{
  tree * view_leaf =  malloc(sizeof(tree));
  view_leaf = root;
  if (view_leaf->l==NULL || view_leaf->r ==NULL)
  {
    printf("No more predicates remaning!\n");
    return 0;
  }
  while(view_leaf->l->l!=NULL)
  {
    view_leaf = view_leaf->l;
  }
  printf("making predicate\n");
  pred = malloc(sizeof(predicates));
  predicates * pred =  make_predicate(view_leaf->l->rel, view_leaf->l->col, view_leaf->r->rel, view_leaf->r->col, '=');
//exec pred
  printf("now we will execute predicate %d.%d = %d.%d",pred->rel1,pred->col1,pred->rel2,pred->col2)
  free(view_leaf->l);
  view_leaf->rel= view_leaf->r->rel;
  view_leaf->col = view_leaf->r->col;
  free(view_leaf->r);


}


int connected(predicates ** predicates, int num_predicates, int rel1, int col1, int rel2, int col2)
{
  int i;
  for (i=0;i<num_predicates;i++)
  {
    if ((predicates[i]->rel1 == rel1) && (predicates[i]->col1==col1) && (predicates[i]->op=='=') && (predicates[i]->rel2 = rel2) && (predicates[i]->col2==col2))
      {
        printf("connected\n");
        return 1;
      }
  }
  return 0;
}
//function to print the array
void printarray(exec_information ** store_information, int * store_info_counter, relation_data ** relations, predicates ** predicates, int num_predicates, int ** arr, int size)
{
    int i,j;
    for(i=0; i<size; i++)
    {
        printf("{%d.%d}\t",arr[i][0], arr[i][1]);
        if (i!=size-1)
        if (connected(predicates, num_predicates,arr[i][0], arr[i][1], arr[i+1][0], arr[i+1][1])==0)
          {
            printf("!!!oou, den einai connected!!!");
            return;
          }
    }
//edw tha dimourgisei to tree based on pinaka arr
    printf("Now creating tree. O theos boithos\n");
    tree * cost_tree = malloc(sizeof(tree));
    cost_tree = new(relations, arr[0][0], arr[0][1], arr[1][0], arr[1][1]);
    for(i=2; i<size; i++)
    {
      cost_tree = insert (relations, cost_tree,arr[i][0],arr[i][1]);
    }
    store_information[store_info_counter]->exec_tree=cost_tree;
    store_information[store_info_counter]->total_cost=calculate_total_cost(cost_tree);
    store_info_counter++;
    restore_data(cost_tree,relations);
}
//function to swap the variables
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
void permutation(exec_information ** store_information, int * store_info_counter, relation_data ** relations,predicates ** predicates, int num_predicates, int ** arr, int start, int end)
{
    int i;
    if(start==end)
    {
        printarray(store_information,store_info_counter, relations, predicates, num_predicates, arr, end+1);
        return;
    }
    for(i=start;i<=end;i++)
    {
        swap(arr, i,start);
        permutation(arr, start+1, end);
        swap(arr, i, start);
    }
}
void find_permutations (exec_information ** store_information, predicates ** predicates, int num_predicates, int ** join_table, int size)
{
  int i,x=size;
  store_information = malloc(sizeof(exec_information*));
  for (i=size-1;i--;i>0)
    x = x * i;//we wiil make x trees
  for (i=0;i++;i<x)
    {
     store_information[i] = malloc(sizeof(exec_information));
     store_information[i]->exec_tree = malloc(sizeof(tree));
     store_information[i]->total_cost = -1;
    }
    int * store_info_counter=0;
    permutation(store_information, store_info_counter, relations, predicates,  num_predicates, join_table, 0, size-1);
    int selected_tree = find_min_cost(store_information);
    printf("We have chosen a tree with cost %d",store_information[selected_tree]->total_cost);
    execute (store_information[selected_tree]->exec_tree);

    for (i=0;i++;i<x)
      {free (store_information[i]->exec_tree);
       free (store_information[i]);
      }
}

int find_min_cost (exec_information ** store_information)
{
  printf("finding total cost\n");
  int min = store_information[0]->total_cost;
  int pos =0;
  int i=1;
  while(store_information[i]->total_cost!=-1)//for all info
  {
    if (store_information[i]->total_cost<min)
      {min = store_information[i]->total_cost;
        pos = i;
      }
      i++;
  }
  return pos;
}
