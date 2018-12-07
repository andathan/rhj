#include "inbetween.h"
#include "rhj.h"
#include "predicates.h"
#include <string.h>
#include <math.h>
struct query{
  char * sxeseis;
  predicates ** katigorimata;
  int num_of_predicates;
  char * proboles;
};
typedef struct query query;

struct batch{
    query ** queries_table;
    int num_of_queries;
};
typedef struct batch batch;
int select_predicate (int numofPredicates, predicates ** input_predicates,all_data * datatable ) //girnaei ton arithmo me to predicate pou prepei na ektelesoume prota. theloume prota auta pou tha bgoun grigora
{
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
  for (int j=0;j<numofPredicates;j++)
  {
    if ((input_predicates[j]->rel1 == -1 || input_predicates[j]->rel2 == -1) && input_predicates[j]->op!='.')
    {
    no_more_filters = 0;
    break;
    }
  }
  if (no_more_filters==1)//teleiosame me ta filters, pame sta joins
    select_filters = 0;
  for (i=0;i<numofPredicates;i++)
  {
    if (select_filters==1)
    {
      if ((input_predicates[i]->rel1 == -1 || input_predicates[i]->rel2 == -1) && input_predicates[i]->op != '.'  ) //einai filtro && den exei ektelestei
      {
        if (input_predicates[i]->rel1==-1)
          {
            constant = input_predicates[i]->col1;
            rel = input_predicates[i]->rel2;
            col = input_predicates[i]->col2;
          }
        else
          {
            constant = input_predicates[i]->col2;
            rel = input_predicates[i]->rel1;
            col = input_predicates[i]->col1;
          }
        if (input_predicates[i]->op == '<')
          {
          distance_to_filter =   datatable->table[rel]->columns[col]->max - constant;
            weight = datatable->table[rel]->columns[col]->spread  * distance_to_filter;
          }
        else if (input_predicates[i]->op =='>')
            {
              distance_to_filter = datatable->table[rel]->columns[col]->min -constant;
              weight = datatable->table[rel]->columns[col]->spread  * distance_to_filter;
            }
        else
          {
              weight = datatable->table[rel]->columns[col]->spread;
          }
        if (distance_to_filter<0)//theloume apoliti timi
          {
            distance_to_filter= distance_to_filter *(-1);
          }
          if (weight<min_weight || min_weight==-1)//an exoume proti epanalipsi
            {

              min_weight = weight;
              return_predicate = i;
            }
          }
    }
    if (select_filters == 0)//den exw alla filtra h exw metaksi sxeseon
    {
      if ((input_predicates[i]->rel1 != -1 &&  input_predicates[i]->rel2 != -1) && input_predicates[i]->op != '.' ) // join pou den exei ginei
      {
            return_predicate = i;
      }
    }
  }
    return return_predicate;
}

int main (void)
{
  int r1,r2,c1,c2;
 inbet_list *List1,*List2=NULL;
 Relation *rel1,*rel2;
  inbetween_results *inb_results = NULL;
   inbet_list *result1,*result2=NULL;
  Relation * relation,* relation2;
  char * tokens[3];
  int i;
  int num_of_queries,num_relations_in_query;
  int num_predicates;
  int predicate_to_execute;
      int constant;
  batch * input_batch;
  predicates **predicates_table;
  relation_data ** arrays_in_query;
//  char input[80] = "0 1 2|0.1=1.2&1.0=2.1&2.1>1821|0.0 1.1";
  char *filename=NULL;
  size_t linesize;

  all_data *datatable =(all_data *)malloc(sizeof(all_data));
  datatable->num_relations=0;
  datatable->table=NULL;
//  datatable->table = (relation_data **)malloc(sizeof(relation_data *));
  getline(&filename,&linesize,stdin);
  while(strcmp(filename,"done\n")!= 0){
    datatable->table = realloc(datatable->table,sizeof(relation_data *)*(datatable->num_relations+1));
    filename[strlen(filename)-1]='\0';
    datatable->table[datatable->num_relations] = parsefile(filename);
    datatable->num_relations++;
    getline(&filename,&linesize,stdin);
  }
getline(&filename,&linesize,stdin);
while (filename!= EOF)
{
  //new query batch
  input_batch = (batch*)malloc(sizeof(input_batch));
  input_batch->num_of_queries=0;
  while(strcmp(filename,"F\n"))//read queries
  {
    seperate_predicate(filename,tokens);
    printf("1:%s\n2:%s\n3:%s\n",tokens[0],tokens[1],tokens[2]);
    num_predicates = find_num_of_predicates(tokens[1]);
    printf("We have %d predicate\n",num_predicates);
    (input_batch->num_of_queries)++;
    input_batch->queries_table = realloc(input_batch->queries_table,sizeof(query*)*input_batch->num_of_queries);
    input_batch->queries_table[input_batch->num_of_queries-1] = realloc(input_batch->queries_table[input_batch->num_of_queries-1],sizeof(query));
    input_batch->queries_table[input_batch->num_of_queries-1]->sxeseis = malloc(strlen(tokens[0]));
    input_batch->queries_table[input_batch->num_of_queries-1]->proboles = malloc(strlen(tokens[2]));
    strcpy(input_batch->queries_table[input_batch->num_of_queries-1]->sxeseis,tokens[0]);
    arrays_in_query= find_corresponding(input_batch->queries_table[input_batch->num_of_queries-1]->sxeseis,datatable,&num_relations_in_query);
    inb_results = InitInbetResults(num_relations_in_query);
    strcpy(input_batch->queries_table[input_batch->num_of_queries-1]->proboles,tokens[2]);
    input_batch->queries_table[input_batch->num_of_queries-1]->katigorimata= realloc (input_batch->queries_table[input_batch->num_of_queries-1]->katigorimata, sizeof(predicates*) * num_predicates);
    input_batch->queries_table[input_batch->num_of_queries-1]->num_of_predicates = num_predicates;
    input_batch->queries_table[input_batch->num_of_queries-1]->katigorimata = fill_predicates(tokens[1],num_predicates);
    //execute query
    for(int k=0;k<num_predicates;k++)
     {
       predicate_to_execute = select_predicate (num_predicates, input_batch->queries_table[input_batch->num_of_queries-1]->katigorimata, datatable );
       printf("Executing predicate %d\n",predicate_to_execute);
       i = predicate_to_execute;
        input_batch->queries_table[input_batch->num_of_queries-1]->katigorimata[predicate_to_execute]->op = '.';
        predicates_table=input_batch->queries_table[input_batch->num_of_queries-1]->katigorimata;
        printf("predicate : %d.%d%c%d.%d\n",predicates_table[i]->rel1,predicates_table[i]->col1,predicates_table[i]->op,predicates_table[i]->rel2,predicates_table[i]->col2);
        r1 =  predicates_table[i]->rel1;
        r2 = predicates_table[i]->rel2;
        c1 =  predicates_table[i]->col1;
        c2 =  predicates_table[i]->col2;

        if(r1==-1){
          constant = predicates_table[i]->col1;
          List2=inb_results->inbet_lists[r2];
          rel2 = arrays_in_query[r2]->columns[c2];

          if(List2->head==NULL){
            relation = rel2;
          }else{
            relation = BuildRelation(List2,rel2);
          }
          result2 = InitInbetList();
          compute_operation(predicates_table[i]->op,constant,relation,result2);
          inb_results = UpdateInbetList2(inb_results,result2,r2);
        }else if(r2==-1){
          constant = predicates_table[i]->col2;
          List1=inb_results->inbet_lists[r1];
          rel1 = arrays_in_query[r1]->columns[c1];
          printf("rel 1 = %d , const = %d \n", predicates_table[i]->rel1,predicates_table[i]->col2);
          if(List1->head==NULL){
            printf("relation %d has no previous results\n",predicates_table[i]->rel1);
            relation = rel1;
          }else{
            relation = BuildRelation(List1,rel1);
            printf("relation %d has previous results\n",predicates_table[i]->rel1);
            printf("BuildRelation\n");
          }
          result1 = InitInbetList();
          compute_operation(predicates_table[i]->op,constant,relation,result1);
          inb_results = UpdateInbetList2(inb_results,result1,r1);
        }else if(predicates_table[i]->rel1==predicates_table[i]->rel2){
          printf("%d==%d\n", predicates_table[i]->rel1,predicates_table[i]->rel2);
          //edo prepei na mpei isotita pinakwn - kalitera se sinartisi
        }else{
          List1=inb_results->inbet_lists[r1];
          List2=inb_results->inbet_lists[r2];
          rel1 = arrays_in_query[r1]->columns[c1];
          rel2 = arrays_in_query[r2]->columns[c2];
          result1 = InitInbetList();
          result2 = InitInbetList();
          if(List2->head!=NULL){
            rel2 = BuildRelation(List2,rel2);
            printf("list2 has been reiinited\n" );
            printf("relation %d has no previous results\n",predicates_table[i]->rel2);
            printf("BuildRelation\n");
          }
          if(List1->head!=NULL){
            printf(" !! %d ,%d\n",r1,List1->head->rowIDS[0] );
            rel1 = BuildRelation(List1,rel1);
            printf("relation %d has previous results\n",predicates_table[i]->rel1);
            printf("BuildRelation\n");
          }
          if(predicates_table[i]->op=='='){
            RadixHashJoin(rel1,rel2,result1,result2);
            printf("kano join\n" );
            printf("total tuples tou 2: %d\n",result2->total_tuples );
            inb_results=UpdateInbetList(inb_results,result1,result2,r1,r2);
            printf("---> %d\n",inb_results->inbet_lists[r2]->joined );
          }else{
            //edo prepei na mpoun ta > , < metaksi pinakwn - sinartisi
            printf("%c\n",predicates_table[i]->op );
          }
        }
  }
     show_results(inb_results,arrays_in_query,input_batch->queries_table[input_batch->num_of_queries-1]->proboles);
    getline(&filename,&linesize,stdin);
  }
//  for (int k=0;k<input_batch->num_of_queries;k++)
//    print_predicates(input_batch->queries_table[k]->katigorimata,input_batch->queries_table[k]->num_of_predicates);
      free(input_batch);
      printf("Batch ended! Waiting for new batch!\n");
      getline(&filename,&linesize,stdin);
  }
   free (arrays_in_query); //to coressponding table xriazete?
}
