typedef struct job{
	void (*function)(void* arg);
	void* arg;
  job *next;
} job;

typedef struct job_queue{
  job *first;
  job *last;
  int n_jobs;
}job_queue;


typedef struct{
  pthread_t *threads;
  int *threads_id;
  int n_threads;
  job_queue queue;
} thpool;

int THP_Init(int n_threads);
