0.
struct relation_data{
  uint64_t numColumns
  uint64_t numTuples
  Relation **columns     //pinakas apo deiktes stin arxi kathe column
}

struct all_data{
  relation_data * table
  int counter
}

struct predicates{
  int rel1;
  int col1;
  int rel2;
  int col2;
  char[3] op;
}

1. perasma pinakwn sti mnimi
  all_data *my_data;
  my_data = malloc
  my_data->counter=0
  uint64_t numColumns
  diabazoume apo to stdin onomata arxeiwn
  h eisodos termatizei otan dothei i leksi done
  file_name=getline apo stdin
  Relation *current_relation
  while(file_name!=DONE){
    my_data->table = realloc
    open file_name
    //numTuples|uint64_t numColumns|uint64_t T0C0|uint64_t T1C0|..|uint64_t
    //TnC0|uint64_t T0C1|..|uint64_t TnC1|..|uint64_t TnCm
    //diabazo apo to arxeio :
    my_data->table[my_data->counter]->numColumns = numColumns
    my_data->table[my_data->counter]->numTuples = numTuples
    my_data->table[my_data->counter]->columns = (Relation **)malloc(sizeof(Relation *)*numColumns)
    my_data->counter++

    for i apo 0 mexri numColumns{
      current_relation = malloc(Relation)
      current_relation->tuples = malloc(sizeof(tuple)*numTuples)
      for j apo 0 mexri numTuples{
        current_relation->tuples[j]->key=j
        current_relation->tuples[j]->payload=timi_apo_arxeio //diabazo apo to arxeio , prepei na simbadisume me to format
      }
      my_data->table[my_data->counter]->columns[i] = current_relation
    }
    my_data->counter++
  }

//se auto to simio exo perasei ola ta arxeia sto my_data
sleep(1)
2. queries
//erxontai se batches , to batch teliwnei otan brw to F
  char **batch;
  int num_queries = 0
  query =getline apo stdin
  //mexri na teleiosoun ta batches:
  while(query!=F){
    queries  = realloc
    queries[num_queries] = auto pou diabasa apo getline
    num_queries++
  }

  //exo apothikeusi ta query
  for i apo 0 mexri num_queries{
    execute(query)
  }

//0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1
struct stepping_list{
  int rowIDS[BUFFERSIZE]
  int counter
  stepping_list *next;
}

struct stepping_stone{
  stepping_list **stepping_arrays
  int total;
}

3.execute(query){

  char *tokens[3]; //ta 3 kommatia tou query
  int k = 0 ;
  tokens[k] = strtok(query,"|\n");
  while(k<2){
    k++;
    tokens[k]=strtok(NULL,"|\n")
  }
  //tokens[0] = pinakes pou simmetexoun
  //tokens[1] = katigorimata
  //tokens[2]= proboles

  num_arrays_in_query = ((strlen(tokens[0]) - 1 )div 2 ) + 1  // -1 gia to /0 , div 2 giati ta misa einai kena , +1 giati panta monos
  //desmeuo treis deiktes se relation data gia na kratiso deiktes apo to arxiko array
  //bazo diplo deikti giati to posa arrays simmetexoun sto query einai metablito megethos
  relation_data **arrays_in_query = malloc(sizeof(relation_data *)*num_arrays_in_query); 
  //sto stp_stone kratao tous endiamesous pinakes gia kathe relation pou simmetexei
  //stp_stone->stepping_array[0] endiamesos pinakas gia to 1o
  //stp_stone->stepping_array[1] endiamesos pinakas gia to 2o k.o.k
  stepping_stone *stp_stone;
  stp_stone = malloc(sizeof(stepping_stone))
  stp_stone ->stepping_arrays = malloc(sizeof(stepping_list *) * num_arrays_in_query)
  for i apo 0 mexri arrays_in_query{
    stp_stone ->stepping_arrays[i]=NULL;
    position = atoi(tokens[0][0]) //sto 1o token tou query , blepo se poious pinakes anaferete kai kratao antistoixisi ston arrays_in_query
    //p.x. an query=0 2 4|...
    //arrays_in_query[0]=0
    //arrays_in_query[1]=2
    //arrays_in_query[2]=4 , kai kratao deiktes ston arxiko
    arrays_in_query[i] = my_data->table[position]
    tokens[0]+=2
  }

  // edo pou eimai me to arrays_in_query exo prosbasi sta dedomena tou pinakas
  // o arrays_in_query exei megethos oso oi pinakes pou simmetexoun
  int num_predicates =0;
  //metrao ta & gia na do posa predicates exo
  for(i=0;i<strlen(tokens[1])-1;i++){ //edo tha baloume -4
    if(tokens[1][i] == '&') num_predicates++;
  }
  num_predicates++;
  //predicates = pinakas me deiktes se predicate
  struct predicate **predicates = malloc(sizeof(predicate *)*num_predicates);

  for i apo 0 mexri num_predicates{
    predicates[i] = malloc(sizeof(predicate))
    if(sscanf(tokens[1],"%d.%d%s%d.%d",predicates[i]->rel1,predicates[i]->col1,predicates[i]->op,predicates[i]->rel2,predicates[i]->col2)==5)
      ;
    else if(sscanf(tokens[1],"%d.%d%s%d",predicates[i]->rel1,predicates[i]->col1,predicates[i]->op,predicates[i]->col2)==4) {
      predicates[i]->rel2=-1;
    }else if(sscanf(tokens[1],"%d%s%d.%d",predicates[i]->col1,predicates[i]->op,predicates[i]->rel2,predicates[i]->col2)==4)
      predicates[i]->rel1=-1;
    else{
      printf(" No Match kati pige lathos\n");
    }
  }

  // peripou to idio gia to tokens[2] pou exei tis proboles
  //pao gia execute
  for(i=0;i<num_predicates;i++){
    if(!strcmp(predicates[i]->op,"=")){
      if(predicates[i]->rel1 == -1 || predicates[i]->rel2 == -1  ){
        if(predicates[i]->rel1==-1){    //den exo join
          if(stepping_stone->stepping_arrays[predicates[i]->rel1] != NULL ){
            relation = build_relation(stepping_stone->stepping_arrays[predicates[i]->rel1],arrays_in_query[predicates[i]->rel1]->columns[col1])
            constant = col2
          }else{
            relation = arrays_in_query[predicates[i]->rel1]->columns[col1]
            constant=col2 // o arithmos pou tha sigkrinw
          }
        }else{ //an den einai to rel1 einai to rel2 = -1
          if(stepping_stone->stepping_arrays[predicates[i]->rel1] != NULL ){
            relation = build_relation(stepping_stone->stepping_arrays[predicates[i]->rel2],arrays_in_query[predicates[i]->rel2]->columns[col2])
            constant = col1
          }else{
            relation = arrays_in_query[predicates[i]->rel2]->columns[col2]
            constant = col1
          }
        }
        if(!strcmp(predicates[i]->op,"<")){
            equalto(relation,constant)
        }
        //aplo filtro , sarwsi
      }else if(predicates[i]->rel1 == predicates[i]->rel2){
        //antistoixa prepei na tsekaro an to stepping_arrays==NULL gia na kano build_relation
        //sarwsi , idio relation
      }else{

        //gia to proto
        if(stepping_stone->stepping_arrays[predicates[i]->rel1] != NULL ){
            Relation *relation1 = build_relation(stepping_stone->stepping_arrays[predicates[i]->rel1],arrays_in_query[predicates[i]->rel1]->columns[col1])
            //dld dinw ta endiamesa apotelesmata kai ton arxiko pinaka gia na mu ftiaksei ena relation
        }else{
          relation1= arrays_in_query[predicates[i]->rel1]->columns[col1]
        }

        //gia to deutero
        if(stepping_stone->stepping_arrays[predicates[i]->rel1] != NULL ){
            Relation *relation2 = build_relation(stepping_stone->stepping_arrays[predicates[i]->rel2],arrays_in_query[predicates[i]->rel2]->columns[col2])
            //dld dinw ta endiamesa apotelesmata kai ton arxiko pinaka gia na mu ftiaksei ena relation
        }else{
          relation2 = arrays_in_query[predicates[i]->rel2]->columns[col2]
        }
        //telika kano join
        RadixHashJoin (relation1,relation2)
      }
    }else{  // eimai se < h >
      if(predicates[i]->rel1==-1){
        if(stepping_stone->stepping_arrays[predicates[i]->rel1] != NULL ){
          relation = build_relation(stepping_stone->stepping_arrays[predicates[i]->rel1],arrays_in_query[predicates[i]->rel1]->columns[col1])
          constant = col2
        }else{
          relation = arrays_in_query[predicates[i]->rel1]->columns[col1]
          constant=col2 // o arithmos pou tha sigkrinw
        }
      }else{ //an den einai to rel1 einai to rel2 = -1
        if(stepping_stone->stepping_arrays[predicates[i]->rel1] != NULL ){
          relation = build_relation(stepping_stone->stepping_arrays[predicates[i]->rel2],arrays_in_query[predicates[i]->rel2]->columns[col2])
          constant = col1
        }else{
          relation = arrays_in_query[predicates[i]->rel2]->columns[col2]
          constant = col1
        }
      }
      if(!strcmp(predicates[i]->op,"<")){
          lessthan(relation,constant)
      }else if(strcmp(predicates[i]->op,">")){
        greaterthan(relation,constant)
      }
  }
}


//mas menei :
//1. na ananewnoyme to stepping_stone
//2. ta results
//3. tin seira pou tha ekteloume ta num_predicates
//4.
