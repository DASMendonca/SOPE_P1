// PROGRAMA p01a.c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define TIME_TO_SLEEP 15

void sigint_handler(int signo)
{
    if(signo == SIGINT)
         printf("\nIn SIGINT handler ...\n");

    if(signo== SIGUSR1)
        printf("SIGUSR1 handled\n");
}


int main(void)
{
    if (signal(SIGINT,sigint_handler) < 0)
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
