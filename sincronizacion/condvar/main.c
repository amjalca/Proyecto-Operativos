#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS  3
#define TCOUNT 10
#define COUNT_LIMIT 12

int     count = 0;
int     thread_ids[4] = {0,1,2,3};
pthread_mutex_t count_mutex;  //mutex for count increase critical section
pthread_cond_t count_threshold_cv; //condition variable

void *inc_count(void *t); //thread function that increases a counter
void *watch_count(void *t); //thread function that wakes up when the counter reaches COUNT_LIMIT
void *imprime(void *t);

int main (int argc, char *argv[]){
  int i, rc, numHilos = 0,numHilosC = 0,numHilosB = 0,numHilosA = 0,numHilosTotal = 0;
  long t1=1, t2=2, t3=3; //internal thread id
  pthread_t threads[NUM_THREADS],threadsCBA[numHilosTotal];
  pthread_attr_t attr;


  printf("ingrse el numero de hilos\n");
  scanf("%i", &numHilos);

  numHilosC = numHilos;
  numHilosB = numHilos/2;
  numHilosA = numHilos/4;

  if(numHilosA == 0){
    numHilosA = 1;
  }

  numHilosTotal = numHilosA+numHilosB+numHilosC;
  /* Initialize mutex and condition variable objects */
  pthread_mutex_init(&count_mutex, NULL);
  pthread_cond_init (&count_threshold_cv, NULL);

  /* Create threads in a joinable state */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  /*
  pthread_create(&threads[0], &attr, watch_count, (void *)t1);
  pthread_create(&threads[1], &attr, inc_count, (void *)t2);
  pthread_create(&threads[2], &attr, inc_count, (void *)t3);
  */

  char prioridad = '\0';
  for(i = 0 ; i < numHilosC ; i++){
    prioridad = 'c';
    printf("CCCCCCCCCCCCCCCC\n");
    pthread_create(&threadsCBA[i], &attr, imprime, (void *) i);
  }

  for(i = numHilosC ; i < numHilosC+numHilosB ;i++){
    prioridad = 'b';
    printf("BBBBBBBBBBBBBBBB\n");
    pthread_create(&threadsCBA[i], &attr, imprime, (void *) i);
  }

  for(i = numHilosC+numHilosB ; i < numHilosC+numHilosB+numHilosA ; i++){
    prioridad = 'a';
    printf("AAAAAAAAAAAAAAAA\n");
    pthread_create(&threadsCBA[i], &attr, imprime, (void *) i);
  }

  /* Wait for all threads to complete */
  for (i=0; i<numHilosTotal; i++) {
    //printf("%i\n", i);
    pthread_join(threadsCBA[i], NULL);
  }

  /* Clean up and exit */
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&count_mutex);
  pthread_cond_destroy(&count_threshold_cv);
  pthread_exit(NULL);

}

void *imprime(void *t){
  long c = (long) t;
  printf("%ld\n", c);
}

void *inc_count(void *t)
{
  int i;
  long my_id = (long)t;

  printf("Starting thread %ld\n", my_id);
  for (i=0; i<TCOUNT; i++) {
    pthread_mutex_lock(&count_mutex);
    count++;

    if (count == COUNT_LIMIT) {
      pthread_cond_signal(&count_threshold_cv);
      printf("thread %ld, count = %d  Threshold reached.\n", my_id, count);
      }
    printf("thread %ld, count = %d, unlocking mutex\n",  my_id, count);
    pthread_mutex_unlock(&count_mutex);

    sleep(1);
    }
  pthread_exit(NULL);
}

void *watch_count(void *t)
{
  long my_id = (long)t;

  printf("Starting thread %ld\n", my_id);

  pthread_mutex_lock(&count_mutex);
  while (count<COUNT_LIMIT) {
    printf("thread %ld suspended.\n", my_id);
    pthread_cond_wait(&count_threshold_cv, &count_mutex);
    printf("thread %ld Condition signal received.\n", my_id);
    count += 125;
    printf("thread %ld count now = %d.\n", my_id, count);
    }
  pthread_mutex_unlock(&count_mutex);
  pthread_exit(NULL);
}