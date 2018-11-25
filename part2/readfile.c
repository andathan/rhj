#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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



relation_data *parsefile(char * filename){

  uint64_t num;
  uint64_t numofColumns;
  uint64_t numofTuples;

  FILE *fp = fopen(filename,"rb");
  if (fp==NULL) {
    printf("yes %s\n",filename);
  }
  fread(&numofTuples,sizeof(uint64_t),1,fp);
  fread(&numofColumns,sizeof(uint64_t),1,fp);

  relation_data *r2data =(relation_data *)malloc(sizeof(relation_data));
  r2data->columns = (Relation **)malloc(sizeof(Relation *)*numofColumns);
  for(int i=0;i<numofColumns;i++){
    r2data->columns[i] =(Relation *)malloc(sizeof(Relation));
    r2data->columns[i]->tuples = (tuple *)malloc(sizeof(tuple)*numofTuples);
  }
  printf("%ju\n",numofTuples );
  printf("%ju\n",numofColumns );
  for(int i=0;i<numofColumns;i++){
    for(int j=0;j<numofTuples;j++){
      fread(&(r2data->columns[i]->tuples[j].payload),sizeof(int32_t),1,fp);
      r2data->columns[i]->tuples[j].key = j;
    }
  }
  /*for(int i=0;i<numofColumns;i++){
    for(int j=0;j<numofTuples;j++){
      printf("%d |%d\n", r2data->columns[i]->tuples[j].key,r2data->columns[i]->tuples[j].payload);
    }
  }*/


  return r2data;
}
int main(int argc,char **argv){
  char *filename=NULL;
  size_t linesize;
  all_data *data =(all_data *)malloc(sizeof(all_data));
  data->num_relations=0;
  data->table = (relation_data **)malloc(sizeof(relation_data *));

  getline(&filename,&linesize,stdin);
  while(strcmp(filename,"done\n")!= 0){
    data->table = realloc(data->table,sizeof(relation_data *)*(data->num_relations+1));
    filename[strlen(filename)-1]='\0';
    data->table[data->num_relations] = parsefile(filename);
    getline(&filename,&linesize,stdin);
  }
}
