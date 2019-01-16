#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



threadpool *THP_Init(int n_threads){
  int i;
  threadpool *thpool = (threadpool *)malloc(sizeof(threadpool));
  thpool->threads = (pthread_t **)malloc(sizeof(pthread_t *)*n_threads);
  thpool->threads_id =(int *)malloc(sizeof(int)*n_threads);

  thpool->queue = JQ_Init();

  for(i=0;i<n_threads;i++){
    thpool->threads[i] = (pthread_t *)malloc(sizeof(pthread_t));
    thpool->threads_id[i] = pthread_create(thpool->threads[i], NULL, (void *)ThreadJob, thpool);
  }
	pthread_mutex_init (&mtx_queue,0);
	pthread_mutex_init (&empty_queue_mtx,0);
	pthread_cond_init (&empty_queue,0);
}

void THP_AddJob(threadpool *thp,void (*function)(void* arg),void *arg){
  if(thp==NULL || thp->queue == NULL) return;
  job *newjob = (job *)malloc(sizeof(job));
  newjob->function = function;
  newjob->arg = arg;
  pthread_mutex_lock(&mtx_queue) ;
  if(jq->n_jobs == 0){ //first job
    jq->first = jq->last = newjob;
  }else{
    jq->last->next = newjob;
    jq->last = jq->last->next;
  }
  jq->n_jobs++;
	pthread_cond_signal(&empty_queue);
	pthread_mutex_unlock(&mtx_queue) ;
  //unlock mutex
}

job_queue *JQ_Init(){
  job_queue *jq = (job_queue *)malloc(sizeof(job_queue));
  jq->first = NULL;
  jq->last = NULL;
  jq->n_jobs=0;
}

void ThreadJob(threadpool *thpool){
	void (*function)(void*);
	void *arg;
	job *my_job = GetJob(thpool);
	function = my_job->function;
	arg = my_job->arg;
	function(arg);
}

job *GetJob(threadpool *thpool){
	job *j;
	if(thpool->queue->n_jobs==0){
		pthread_cond_wait(&empty_queue, &empty_queue_mtx );
	}
	pthread_mutex_unlock(&empty_queue_mtx);
	pthread_mutex_lock(&mtx_queue);
	j = thpool->queue->first;
	thpool->queue->first = j->next;
	pthread_mutex_unlock(&mtx_queue);
	return j;
}
