/*
 File: thread_managment.c 
 -------------------------------------
 Author:  Jack Waslen
 -------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_RESOURCES 5
int available_resources = MAX_RESOURCES;
pthread_mutex_t lock;

// 0 is false, 1 true
int available = 1;
#define NUM_THREADS 5

/* decrease available_resources by count resources */
/* return 0 if sufficient resources available*/
/* otherwise return -1 */
int decrease_count(int thread_number, int count){
    if (available_resources < count){
        return -1;
    } else {
        available_resources -= count;
        printf("The thread %d has acquired, %d resources and %d more resources are available. \n", thread_number, count, available_resources);
        return 0;
    }
}

/* increase available_resources by count*/
int increase_count(int thread_number, int count){
    available_resources += count;
    printf("The thread %d has released, %d resources and %d resources are now available. \n",thread_number, count, available_resources);
    return 0;
}


void* thread_function(void* THREAD_NUMBER){
    int thread = *((int *) THREAD_NUMBER);
    free(THREAD_NUMBER);
   // printf("Thread #: %d\n", thread);
    int result = -1;
    if(pthread_mutex_lock(&lock) == 0){
       // printf("Lock is : %d, decreasing count\n",0);
        result = decrease_count(thread, 1);

        pthread_mutex_unlock(&lock);
    }



    if (result == 0){
       
        sleep(1);
        if (pthread_mutex_lock(&lock) == 0){
         
           
            increase_count(thread, 1);
            pthread_mutex_unlock(&lock);
        }


    } else {
        printf("Thread %d could not acquire enough resources.\n",thread);
    }
  
    return 0;
    
}
int main(){
    // call function void* thread_function (THREAD_NUMBER)
   // invokes decrease count, if succeeded in request, sleep for 1 second otherwise

   // after 1s thread returns resources by calling increase_count

   int threadCount = 0;
   pthread_t *threadArray;
   threadArray = malloc(100 * sizeof(*threadArray));
   while (threadCount < NUM_THREADS){
        pthread_t t_id;
        threadArray[threadCount] = t_id;
        pthread_mutex_init(&lock, NULL);
        int *arg = malloc(sizeof(*arg));
        //printf("Thread #: %d\n", threadCount);
        *arg = threadCount;
        pthread_create(&t_id, NULL, thread_function, arg);
     
    
        threadCount += 1;
   }
   threadCount = 0;
   while (threadCount < NUM_THREADS){
      pthread_join(threadArray[threadCount], NULL);
      threadCount +=1;
   }
   
    printf("All threads have finished execution. Available resources: %d\n", available_resources);
    exit(0);
}
