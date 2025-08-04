#include <stdio.h>
#include <pthread.h>
#include "count.h"

// Dam bao nguoi dung khong the doc truc tiep bien count = khoa mutex
// Phai doc gia tri bien count thong qua ham` cung cap tu` truoc

int count;
pthread_mutex_t count_lock;

void count_increase()
{
    count_init();
    pthread_mutex_lock(&count_lock);
    count++;
    pthread_mutex_unlock(&count_lock);
}
int is_init;
void count_init()
{   
    if(0 == is_init)
    {
        pthread_mutex_init(&count_lock, NULL );
        is_init=1;
    }
}