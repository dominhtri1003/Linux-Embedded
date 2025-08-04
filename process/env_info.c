#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main()
{
    char *home_dir;
    char *user_name;
    char current_working_dir[PATH_MAX];

    printf("thong tin bien moi truong va duong dan");

    home_dir = getenv("HOME");
    if (home_dir != NULL)
    {
        printf("duong dan thu muc home: %s\n", home_dir);
    }
    else
    {
        printf("khong tim thay duong dan thu muc home");
    }
    
    user_name = getenv("USER");
    if(user_name != NULL)
    {
        printf("User name: %s\n", user_name);
    }
    else
    {
        printf("Khong tim thay user name");
    }

    if(getcwd(current_working_dir, sizeof(current_working_dir)) != NULL)
    {
        printf("PATH: %s\n", current_working_dir);
    }
    else
    {
        perror("PATH error");
    }
    return 0;
}
