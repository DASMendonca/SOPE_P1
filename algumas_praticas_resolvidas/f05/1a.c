#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>

#define READ 0
#define WRITE 1


int main(void) {

    int fd[2];
    pipe(fd);
    pid_t pid;

    int numbers[2];

    if((pid= fork() > 0)) {
        close(fd[READ]);
        int option = 1;

        while(option != 0) {
            printf("\nInsert number 1:\n");
            scanf("%d", &numbers[0]);
            printf("\nInsert number 2:\n");
            scanf("%d", &numbers[1]);
            write(fd[WRITE], numbers, sizeof(numbers));
            sleep(1);
            printf("\nExit? write 0\n");
            scanf("%d", &option);
        }
        close(fd[WRITE]);
        kill(pid, SIGTERM);




    }
    else{
        while(1){
            close(fd[WRITE]);
            if(read(fd[READ], numbers, sizeof(numbers)) == 0)
                break;
            printf("\nSoma %d + %d= %d\n", numbers[0], numbers[1], (numbers[0]+numbers[1]));
            printf("\nSubtracao %d - %d= %d\n", numbers[0], numbers[1], (numbers[0]-numbers[1]));
        }
    }
    close(fd[READ]);


    return 0;
}
