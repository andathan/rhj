#include <pthread.h>
#define N_THREADS 3


typedef struct job{
	void   (*function)(void* arg);		/*function to be executed*/
	void*  arg;				/*arguments of function*/
  struct job *next;				/*next job*/
} job;

typedef struct job_queue{
  job *first;					/*first job to be executed*/
  job *last;					/*last job to be executed*/
  int n_jobs;					/*number of jobs in queue*/
}job_queue;


typedef struct{
  pthread_t **threads;
  int *threads_id;				/*ids of threads*/
	int jobs2bedone;			/*number of jobs that have to be done*/
	int jobsdone;				/*jobs that have been done*/
  int n_threads;				/*number of threads*/
	int threads_alive;			/*number of threads alive*/
	int threads_working;			/*nummber of threads currently doing a job*/
	job_queue *queue;			/*queue of jobs*/
	pthread_mutex_t barrier_mtx;		/*mutex for condition barrier_cond*/
	pthread_mutex_t working_mtx ;		/*mutex for working threads*/
	pthread_mutex_t empty_queue_mtx;	/*mutex for condition empty_queue*/
	pthread_cond_t empty_queue ;		/*condition to wait if queue has no jobs*/
	pthread_mutex_t mtx_queue ;		/*mutex to insert and pull jobs with safety*/
	pthread_cond_t barrier_cond;		/*condition for barrier*/

	pthread_mutex_t alive_mtx ;		/*mutex for threads_alive*/
	pthread_cond_t alive_cond;		/*condition for threads alive*/

}threadpool;

void ThreadJob(threadpool *thpool);
job *GetJob(threadpool *thpool);
threadpool *THP_Init(int n_threads);
void THP_AddJob(threadpool *thp,void (*function)(void* arg),void *arg);
job_queue *JQ_Init();
void THP_Wait(threadpool *thp);
void THP_Barrier(threadpool *thp);
void THP_Destroy(threadpool *thp);
