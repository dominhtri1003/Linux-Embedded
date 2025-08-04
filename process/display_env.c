#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    extern char **environ;
    printf("Bien moi truong\n");
    for (char **env = environ; *env != NULL; env++)
    {
        char *current_env_string = *env;
        char *equals_sign = strchr(current_env_string, '=');

        if (equals_sign != NULL)
        {
            size_t name_length = equals_sign - current_env_string;
            // Name variable
            printf("Ten bien: %.*s\n", (int)name_length, current_env_string);
            // Value variable
            printf("Gia tri: %s\n", equals_sign + 1);
        }
        else
        {
            printf("Khong the phan tich bien: %s\n", current_env_string);
            printf("----------------------------------\n");
        }
    }
    return 0;
}

    