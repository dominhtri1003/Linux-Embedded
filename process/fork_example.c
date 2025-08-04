#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    const char *child_message = "child is writing\n";
    const char *parrent_message = "parent is writing\n";
    int pid;
    FILE *file_ptr;

    file_ptr = fopen("my_test_file.txt", "a");
    pid = fork();
    if(pid == 0 )
    {
        printf("PID con: %d\n", getpid());
        for(int i = 0; i<5; i++)
        {
            fprintf(file_ptr, "Con (PID %d) ghi lan %d: %s\n",getpid(), i+1, child_message);
            fflush(file_ptr);
        }
        printf("Con (PID %d) da ghi xong vao file.\n", getpid());
        fclose(file_ptr);
        return EXIT_SUCCESS;
    }
    else
    {
        printf("PID cha: %d\n", getpid());
        for (int i = 0; i < 5; i++)
        {
            fprintf(file_ptr, "Cha (PID %d) ghi lan %d: %s\n",getpid(), i+1, parrent_message);
            fflush(file_ptr);
        }
        printf("Cha (PID %d) da ghi xong vao file.\n", getpid());
        fclose(file_ptr);
        return EXIT_SUCCESS;
    }
}
