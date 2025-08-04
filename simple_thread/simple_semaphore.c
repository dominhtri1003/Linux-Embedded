#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t my_semaphores;

void *thread_function(void *arg)
{
    int thread_id = *(int *)arg;
    printf("Thread %d: Waiting for semaphore...\n", thread_id);
    sem_wait(&my_semaphores);
    printf("Thread %d: Aquired semaphore, perfoming work...\n", thread_id);
    printf("Thread %d: Releasing semaphore.\n", thread_id);
    sem_post(&my_semaphores);
    return NULL;
}

int main()
{
    pthread_t thread1;
    pthread_t thread2;
    int id1 = 1, id2 = 2;
    void *thread1_retval;
    void *thread2_retval;

    sem_init(&my_semaphores, 0, 1);

    pthread_create(&thread1, NULL, thread_function, &id1);
    pthread_create(&thread2, NULL, thread_function, &id2);

    pthread_join(thread1, NULL);
    //printf("Thread 1 return %s\n", (char *)thread1_retval);
    
    pthread_join(thread2, NULL);
    //printf("Thread 2 return %s\n", (char *)thread2_retval);
    printf("All thread finished");
    return 0;
}