#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Su dung : %s <ten_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    FILE *file_ptr;

    file_ptr = fopen(filename, "r");

    if(file_ptr == NULL)
    {
        perror("Loi mo file");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Mo file '%s' thanh cong.\n", filename);
        fclose(file_ptr);
        return 0;
    }
}
