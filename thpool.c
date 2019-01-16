#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct job{
	void   (*function)(void* arg);
	void*  arg;
  job *next;
} job;

typedef struct job_queue{
  job *first;
  job *last;
  int n_jobs;
}job_queue;


typedef struct{
  pthread_t **threads;
  int *threads_id;
  int n_threads;
  job_queue *queue;
} threadpool;

threadpool *THP_Init(int n_threads){
  int i;
  threadpool *thpool = (threadpool *)malloc(sizeof(threadpool));
  thpool->threads = (pthread_t **)malloc(sizeof(pthread_t *)*n_threads);
  thpool->threads_id =(int *)malloc(sizeof(int)*n_threads);

  thpool->queue = JQ_Init();

  for(i=0;i<n_threads;i++){
    thpool->threads[i] = (pthread_t *)malloc(sizeof(pthread_t));
    thpool->threads_id[i] = pthread_create(thpool->threads[i], NULL, (void *)ThreadJob, thpool->queue);
  }

}

void THP_AddJob(threadpool *thp,void (*function)(void* arg),void *arg){
  if(thp==NULL || thp->queue == NULL) return;
  job *newjob = (job *)malloc(sizeof(job));
  newjob->function = function;
  newjob->arg = arg;
  //lock mutex
  if(jq->n_jobs == 0){ //first job
    jq->first = jq->last = newjob;
  }else{
    jq->last->next = newjob;
    jq->last = jq->last->next;
  }
  jq->n_jobs++;
  //unlock mutex
}

job_queue *JQ_Init(){
  job_queue *jq = (job_queue *)malloc(sizeof(job_queue));
  jq->first = NULL;
  jq->last = NULL;
  jq->n_jobs=0;
}
