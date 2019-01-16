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
	pthread_mutex_t mtx_queue ;
	pthread_cond_t empty_queue ;
	pthread_mutex_t empty_queue_mtx;
} threadpool;

void ThreadJob(threadpool *thpool);
job *GetJob(threadpool *thpool);
int THP_Init(int n_threads);
void THP_AddJob(threadpool *thp,void (*function)(void* arg),void *arg);
job_queue *JQ_Init();
