#include <stdio.h>
#include <pthread.h>
#include <string.h>

// Ham nay tra ve 1 con tro kieu void *
void *my_thread(void *arg)
{
    printf("Hello world %s\n", (char *)arg);
    return arg;
}

int main()
{
    pthread_t thread1;
    pthread_t thread2;

    void *thread1_retval;
    void *thread2_retval;
    
    // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
    
    // Tham so 1: dia chi thread1, cac thong tin cua thread moi se dien vao struct thread1, luu tru ID cua thread moi tao
    // Tham so 2: thuoc tinh
    // Tham so 3: con tro ham`. Tro toi dia chi cua ham ma` luong moi thuc thi trong lan goi dau tien. la ham main cua thread do
    // Tham so 4L Doi so truyen vao ham my_thread
    // void *arg la` "Tri"
    pthread_create(&thread1, NULL, my_thread, "Tri");
    pthread_create(&thread2, NULL, my_thread, "Do");

    // int pthread_join(pthread_t thread, void **retval);    

    // Ham pthread_join dung` de cho` 1 ham` ket thuc. Luong goi pthread_join se bi block
    // cho den khi luong duoc chi dinh ket thuc viec thuc thi
    // Tham so 1: ID thread muon cho`
    // Tham so 2: Con tro? tro? toi 1 con tro? de nhan gia tri tra? ve` tu` ham` my_thread
    pthread_join(thread1, &thread1_retval);
    printf("Thread 1 return %s\n", (char *)thread1_retval);
    
    pthread_join(thread2, &thread2_retval );
    printf("Thread 2 return %s\n", (char *)thread2_retval);

    return 0;
}