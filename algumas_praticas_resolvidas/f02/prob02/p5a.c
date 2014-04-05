#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
  int fd;
  char *text1="AAAAA";
  char *text2="BBBBB";

  if((fd = open("f1.txt",O_CREAT|O_EXCL|O_TRUNC|O_WRONLY|O_SYNC,0600)) == -1){
    printf("ERROR: %s\n", strerror(errno));
    return -1;
  }
  printf("%d\n", fd);
  getchar();
  write(fd,text1,5);
  getchar();
  write(fd,text2,5);
  close(fd);
  return 0;
}


