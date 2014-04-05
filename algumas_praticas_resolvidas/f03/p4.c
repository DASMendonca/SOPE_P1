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

    __pid_t pid;
    int status;
    pid = fork();
    if(pid == -1){
        perror("Fork error");
        exit(1);
        }

    if(pid==0)
        printf("Hello ");
    else{

        waitpid(pid, &status, 0);
            printf("my friend\n");
    }
    return 0;
}
