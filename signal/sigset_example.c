#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

// Vi su su dung block signal
int main()
{
    sigset_t my_sig_set;
    sigset_t pending_signal;
    
    sigemptyset(&my_sig_set);
    sigaddset(&my_sig_set, SIGINT);
    sigprocmask(SIG_BLOCK, &my_sig_set, NULL);

    while(1)
    {
        sigpending(&pending_signal);
        if(sigismember(&pending_signal, SIGINT))
        {
            printf("Phat hien SIGINT trong pending signal\n");
            sigprocmask(SIG_UNBLOCK, &my_sig_set, NULL);
            return EXIT_SUCCESS;
        }
    }
    return 0;
}