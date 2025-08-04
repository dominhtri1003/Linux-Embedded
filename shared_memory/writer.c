#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

void main()
{
    int shm_fd;
    const char* message_0 = "Hello World!\n";
    void* ptr;

    // Tao file fd dinh danh cho vung nho
    shm_fd = shm_open("Hello", O_CREAT | O_RDWR, 0666);
    // Set size cho vung nho vat ly
    // Vung` share nay` la` vung nho vat ly that
    // Khong the set qua bo nho trong tren vung nho vat ly
    ftruncate(shm_fd, 4096);
    // Mapping tu` vung` nho vat ly vao` dai? dia chi cua chung ta
    // Bat dau tim` kiem tu dia chi 0
    // 4096 o? day phai nho? hon vung nho 4096 o? dung code ftruncate
    // Set map vao` voi permission ghi, map vao` voi che do share, dinh danh fd trung` o tren
    // Map vao tu` offset dau tien
    // Tra? ve dia chi ao, co the truy cap vung` nho ao? thong qua con tro ptr
    ptr = mmap(0, 4096, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    strcpy(ptr, message_0);
    return;

}