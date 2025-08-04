#include <stdio.h>
#include <signal.h>

// Ham xu ly signal - Overwrite CTRL C cua terminal
// Thay vi interrupt process thi se in ra man hinh "Hello world"
void my_signal_handler(int signo)
{
    printf("Hello world\n");
}

void main()
{
    signal(SIGINT, my_signal_handler);
    while(1);
}
