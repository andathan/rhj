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
  curr_pred = make_predicate(r1,c1,r2,c2,'=');
  join->col = update_statistics(relations,curr_pred);//find cost
  join ->l = leaf1;
  join ->r = leaf2;
  return join;
}
tree * insert ( relation_data ** relations, tree * root, int r, int c)
{
  tree * leaf = malloc(sizeof(tree));
  tree * view_leaf =  malloc(sizeof(tree));
  predicates * curr_pred = malloc(sizeof(predicates));
  leaf->rel = r;
  leaf->col = c;
  leaf->l = NULL;
  leaf->r = NULL;
  tree * join = malloc(sizeof(tree));
  join->rel = -1;//join
  view_leaf = root;
  view_leaf = view_leaf->l;
  if (view_leaf->r==NULL)
  curr_pred = make_predicate(view_leaf->rel,view_leaf->col,r,c,'=');
  else
  curr_pred = make_predicate(view_leaf->r->rel,view_leaf->r->col,r,c,'=');
  printf("ftiaksame ena oreotato predicate %d.%d %c %d.%d",curr_pred->rel1,curr_pred->col1,curr_pred->op,curr_pred->rel2,curr_pred->col2);
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
  int sum=0;
  tree * view_leaf =  malloc(sizeof(tree));
  view_leaf = root;
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
    sum+= view_leaf->col;
  }
  printf("total cost is %d",sum);
  return sum;
}

//execute
int execute (tree * root, predicates ** predicate, int numofPredicates)
{
  int i;
  int predicate_to_return=-1;
  tree * view_leaf =  malloc(sizeof(tree));
  tree * trans_leaf = malloc(sizeof(tree));
  trans_leaf = root;
  if (trans_leaf->l==NULL || trans_leaf->r ==NULL)
  {
    printf("No more predicates remaning!\n");
    return -1;
  }
  while(trans_leaf->l!=NULL)
  {
    view_leaf = trans_leaf;
    trans_leaf = trans_leaf->l;
  }
  //find to predicate auto
  for (i=0;i<numofPredicates;i++)
  {
    if ((predicate[i]->rel1==view_leaf->l->rel && predicate[i]->col1==view_leaf->l->col && predicate[i]->rel2==view_leaf->r->rel && predicate[i]->col2==view_leaf->r->col && predicate[i]->op=='=') || (predicate[i]->rel2==view_leaf->l->rel && predicate[i]->col2==view_leaf->l->col && predicate[i]->rel1==view_leaf->r->rel && predicate[i]->col1==view_leaf->r->col && predicate[i]->op=='='))//anapoda
      {
        predicate_to_return  = i;
        printf("We will return predicate %d\n",predicate_to_return);
      }
  }
  if (predicate_to_return==-1)
    printf("paixtike malakia, den brikame to predicate\n\n\n");
  free(view_leaf->l);
  view_leaf->rel= view_leaf->r->rel;
  view_leaf->col = view_leaf->r->col;
  free(view_leaf->r);
  return predicate_to_return;

}


int connected(predicates ** predicate, int num_predicates, int rel1, int col1, int rel2, int col2)
{
  int i;
//  printf("we are trying to find %d.%d = %d.%d|",rel1,col1,rel2,col2);
  for (i=0;i<num_predicates;i++)
  {
//    printf("[Now checking predicate %d.%d %c %d.%d]\n",predicate[i]->rel1,predicate[i]->col1,predicate[i]->op, predicate[i]->rel2,predicate[i]->col2);
    if ((predicate[i]->rel1 == rel1) && (predicate[i]->col1==col1) && (predicate[i]->op=='=') && (predicate[i]->rel2 == rel2) && (predicate[i]->col2==col2)
    || (predicate[i]->rel1 == rel2) && (predicate[i]->col1==col2) && (predicate[i]->op=='=') && (predicate[i]->rel2 == rel1) && (predicate[i]->col2==col1))//h anapoda
      {
  //      printf("connected\n");
        return 1;
      }
  }
  return 0;
}
void permutation_to_trees(exec_information ** store_information, int * store_info_counter, relation_data ** relations, predicates ** predicate, int num_predicates, int ** arr, int size)
{
    int i,j;
    char flag=1;
    for(i=0; i<size; i++)
    {
  //      printf("{%d.%d}\t",arr[i][0], arr[i][1]);
    }
    for(i=0; i<size; i++)
    {
        if (i!=size-1)
        if (connected(predicate, num_predicates,arr[i][0], arr[i][1], arr[i+1][0], arr[i+1][1])==0)
          {
            flag=0;
            return;
          }
    }
    if (flag==1)
    {printf("!!Brikame chain!!!\n----------\n");
    for(i=0; i<size; i++)
    {
        printf("{%d.%d}\t",arr[i][0], arr[i][1]);
    }
    printf("-----------------\n");
  }
//edw tha dimourgisei to tree based on pinaka arr
    printf("Now creating tree. O theos boithos\n");
    tree * cost_tree = malloc(sizeof(tree));
    cost_tree = new(relations, arr[0][0], arr[0][1], arr[1][0], arr[1][1]);
    for(i=2; i<size; i++)
    {
      cost_tree = insert (relations, cost_tree,arr[i][0],arr[i][1]);
    }
    printf("aide kai to balame to tree mas. tora to xonoume stin thesi %d\n\n\n",*store_info_counter);
    store_information[*store_info_counter]->exec_tree=cost_tree;
    store_information[*store_info_counter]->total_cost=calculate_total_cost(cost_tree);
    (*store_info_counter)++;
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
  int i,x=num_predicates;
  exec_information ** store_information;
  for (i=num_predicates-1;i>0;i--)
    x = x * i;//we wiil make x trees
  store_information = (exec_information**)malloc(x*sizeof(exec_information*));
  for (i=0;i<x;i++)
    {
     store_information[i] = (exec_information*)malloc(sizeof(exec_information));
     store_information[i]->exec_tree = (tree*)malloc(sizeof(tree));
     store_information[i]->total_cost = -1;
    }
    printf("Kaname %d mallocs\n",i);
    int * store_info_counter=malloc(sizeof(int));
    *store_info_counter=0;
    permutation(store_information, store_info_counter, relations, predicate,  num_predicates, join_table, 0, num_of_join_pred-1);//edw anti gia num pred -1 mporei na thelei apla na balw "2"
    int selected_tree = find_min_cost(store_information);
    printf("We have chosen a tree with cost %d",store_information[selected_tree]->total_cost);
    for (i=0;i<num_of_join_pred;i++)
    {
        order_of_joins[i] = execute (store_information[selected_tree]->exec_tree, predicate, num_predicates);
    }

    for (i=0;i++;i<x)
      {free (store_information[i]->exec_tree);
       free (store_information[i]);
      }
    return order_of_joins;
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
