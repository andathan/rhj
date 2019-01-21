#include "inbetween.h"
#include "rhj.h"
#include "predicates.h"
#include <string.h>
#include <math.h>


int main (void)
{
  char *tokens[3];
  batch * input_batch;
  char *filename=NULL;
  size_t linesize;
  all_data *datatable =(all_data *)malloc(sizeof(all_data));
  datatable->num_relations=0;
  datatable->table=NULL;

  //  char input[80] = "0 1 2|0.1=1.2&1.0=2.1&2.1>1821|0.0 1.1";
//  datatable->table = (relation_data **)malloc(sizeof(relation_data *));

  getline(&filename,&linesize,stdin);

  while(strcmp(filename,"Done\n")!= 0){
    datatable->table = realloc(datatable->table,sizeof(relation_data *)*(datatable->num_relations+1));
    filename[strlen(filename)-1]='\0';
    datatable->table[datatable->num_relations] = parsefile(filename);
    datatable->num_relations++;
    getline(&filename,&linesize,stdin);
  }
  size_t b;
  b=getline(&filename,&linesize,stdin);
  while(b!=0){
    //new query batch
    input_batch = InitBatch();
    while(strcmp(filename,"F\n")!=0){ //read and store queries in batch
      seperate_predicate(filename,tokens);
      AddToBatch(input_batch,tokens);
      getline(&filename,&linesize,stdin);
    }

    for(int k=0;k<input_batch->num_of_queries;k++){
      execute_query(input_batch->queries_table[k],datatable);
    }
    FreeBatch(input_batch);
    b=getline(&filename,&linesize,stdin);
  }
}
