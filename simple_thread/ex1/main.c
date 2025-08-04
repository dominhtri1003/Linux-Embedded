#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t file_mutex_lock;
FILE *file_ptr;

void write_file_safely(const char *data)
{
    pthread_mutex_lock(&file_mutex_lock);
    fprintf(file_ptr, "%s\n", data);
    fflush(file_ptr);
    pthread_mutex_unlock(&file_mutex_lock);
}

void *thread1_function(void *arg)
{
    for (int i = 0; i < 5; i++)
    {
        char buffer[30];
        snprintf(buffer, sizeof(buffer), "Hello from thread 1 %d", i+1);
        write_file_safely(buffer);
        usleep(100000);
    }
    return NULL;
}

void *thread2_function(void *arg)
{
    for (int i = 0; i < 5; i++)
    {
        char buffer[30];
        snprintf(buffer, sizeof(buffer), "Goodbye from thread 2 %d", i+1);
        write_file_safely(buffer);
        usleep(100000);
    }
    return NULL;
}

int main()
{
    const char *file_name = "ex_thread.txt";
    pthread_t thread1, thread2;
    void *thread_retval1, *thread_retval2;

    file_ptr = fopen(file_name, "w");
    if (file_ptr == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    pthread_mutex_init(&file_mutex_lock, NULL);

    pthread_create(&thread1, NULL, thread1_function, NULL);
    pthread_create(&thread2, NULL, thread2_function, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("All threads finished jobs");
    fclose(file_ptr);
    pthread_mutex_destroy(&file_mutex_lock);

    return 0;
}