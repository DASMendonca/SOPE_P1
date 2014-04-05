// PROGRAMA p2.c
#include "unistd.h"
#include "stdio.h"

int main(void) {
printf("1\n");
//printf("1");
//write(STDOUT_FILENO,"1",1);
if(fork() > 0) {
printf("2");
printf("3");
//write(STDOUT_FILENO,"2",1);
//write(STDOUT_FILENO,"3",1);
} else {
printf("4");
printf("5");
//write(STDOUT_FILENO,"4",1);
//write(STDOUT_FILENO,"5",1);
}
//write(STDOUT_FILENO,"\n",1);
printf("\n");
return 0;
}
