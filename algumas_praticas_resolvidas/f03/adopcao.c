
//fork
#include <unistd.h>
//waitpid
#include <sys/wait.h>
#include <sys/types.h>
//errno
#include <errno.h>
//perror printf
#include <stdio.h>
//exit
#include <stdlib.h>


int main(void){
    __pid_t pid, ppid;

    pid= fork();

    if(pid != 0){
        printf("pai fechou\n");
        return 0;
    }

    printf("%d\n", getppid());
    sleep(1);
    printf("%d\n", getppid());
    while(getppid() != 1){ fprintf(stderr, "."); sleep(1);}

    printf("Fui adoptado pelo init.\n");

    return 0;
}
