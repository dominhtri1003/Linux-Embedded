#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

int count = 0;
pthread_mutex_t count_lock;

void *my_thread(void *arg)
{   
    // Tao 1 pointer kieu toi co vung` nho tren heap, vung` nho nay ko bi huy khi ham` ket thuc
    int *thread_count = malloc(sizeof(int));
    pthread_mutex_lock(&count_lock);   
    for(int i = 0; i < 50000; i++)
    {
        count++;
    }
    // *thread_count: Giai tham chieu pointer thread_count
    // Giai tham chieu cho phep truy cap vao gia tri tai dia chi ma` pointer dang point toi
    // Y nghia cau lenh: Lay gia tri bien count va` luu vao vung` bo nho ma` con tro
    // thread_count dang tro toi
    *thread_count = count;
    pthread_mutex_unlock(&count_lock);

    return (void *)thread_count;
}

int main()
{
    pthread_t thread1;
    pthread_t thread2;

    void *thread1_retval;
    void *thread2_retval;
    
    pthread_mutex_init(&count_lock, NULL);    
    
    pthread_create(&thread1, NULL, my_thread, "Tri");
    pthread_create(&thread2, NULL, my_thread, "Do");

    pthread_join(thread1, &thread1_retval);
    printf("Thread 1 return %d\n", *(int *)thread1_retval);
    // Giai phong vung nho duoc cap phat bang malloc trong luong thread1
    free(thread1_retval);

    pthread_join(thread2, &thread2_retval );
    printf("Thread 2 return %d\n", *(int *)thread2_retval);
    // Giai phong vung nho duoc cap phat bang malloc trong luong thread2ff
    free(thread2_retval);
    
    pthread_mutex_destroy(&count_lock);
    printf("%d\n", count);

    return 0;
}