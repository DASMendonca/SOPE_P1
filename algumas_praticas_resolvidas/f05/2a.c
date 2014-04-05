#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>

#define READ 0
#define WRITE 1

typedef struct Numbers{
        int a;
        int b;
} Numbers;

int main(void) {

    int fd[2];
    int fd2[2];
    pipe(fd);
    pipe(fd2);
    pid_t pid;
    Numbers nums_struct;
    int res;


    if((pid= fork() > 0)) {
        close(fd[READ]);
        close(fd2[WRITE]);
        int option = 1;

        while(option != 0) {
            printf("\nInsert number 1:\n");
            scanf("%d", &nums_struct.a);
            printf("\nInsert number 2:\n");
            scanf("%d", &nums_struct.b);
            write(fd[WRITE], &nums_struct, sizeof(Numbers));
            sleep(1);

            if(read(fd2[READ], &res, sizeof(int))== 0)
                break;

            printf("Sum = %d", res);

            printf("\nExit? write 0\n");
            scanf("%d", &option);
        }
        close(fd[WRITE]);
        close(fd2[READ]);
        kill(pid, SIGTERM);




    }
    else{
        close(fd2[READ]);
        close(fd[WRITE]);

        while(1){
            if(read(fd[READ], &nums_struct, sizeof(Numbers)) == 0)
                break;
            res= nums_struct.a + nums_struct.b;
           if(write(fd2[WRITE], &res, sizeof(int))== 0)
            break;
        }
    }
    close(fd[READ]);
    close(fd2[WRITE]);


    return 0;
}
