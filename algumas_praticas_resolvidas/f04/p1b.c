// PROGRAMA p01a.c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define TIME_TO_SLEEP 15

void sign_handler(int signo)
{
    if(signo == SIGINT)
        printf("\nIn SIGINT handler ...\n");

    if(signo== SIGUSR1)
        printf("SIGUSR1 handled\n");
}



int main(void)
{

    struct sigaction action;
    action.sa_handler = sign_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, NULL) < 0)
    {
        fprintf(stderr,"Unable to install SIGINT handler\n");
        exit(1);
    }
    printf("Sleeping for %d seconds ...\n", TIME_TO_SLEEP);

    int n = TIME_TO_SLEEP;

    while(( n = sleep(n)) != 0);


    printf("Waking up ...\n");
    exit(0);
}
