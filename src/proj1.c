/*
 ============================================================================
 Name        : proj1.c
 Author      : Daniel Mendonça
 Version     :
 Copyright   : Your copyright notice
 Description : Project 1, SOPE
 ============================================================================
 */

#include "proj1.h"

#define TESTING 1
#define READ 0
#define WRITE 1

void intHandler(int sign){
	//TODO
}


int main(int argc, char *argv[]) {

	int run_time;
	__pid_t pid, pgr_id[(argc-3)];


	if(argc<4){
		puts("Invalid input.\nInput example: ./monitor time word filename1 filename2 ... filenameN\n");
		return -1;
	}
	//printf("\n"); //testing

	run_time = atoi(argv[1]);
	//printf("run_time = %d\n", run_time);  //testing

	int i = 3;
	for(; i<argc; i++){
		int tmp = i -3;
		printf("\nprocess nr: %d\n", tmp);

		pid = fork();
		if(pid == 0){
			//word control
			monitorWord(argv[2], argv[i]);
			return 0;
		}
		else{
			__pid_t tmp_pid = pid;
			pgr_id[tmp]= pid;
			setpgid(pid, 0); //child pgrid is the sames as his pid

			//creating monitor_aux for the file and process
			if((pid = fork())==0){
				monitorAux(argv[i]);
				return 0;
			}
			else{
				setpgid(pid, tmp_pid);	//monitor_aux pgrid to the same of monitor_word
				pgr_id[tmp]= tmp_pid;
			}
		}

	}



	return 0;
}


/*
 * check if files still exist every 5sec
 */
void monitorAux(char *filename){
	//TODO
	sleep(1);
	printf("I'm a monitor_aux, my pid is: %d and my pgid is: %d\n", (int) getpid(), (int) getpgrp()); //testing
	printf("%d: I'm watching %s\n", (int) getpid(), filename); //testing
	int fd;
	while(1){
		sleep(5);
		fd = open(filename, O_RDONLY);
		if(fd== -1)
			break;
		else
			close(fd);
	}
	printf("File %s does not exist and will not be tracked\n", filename); //testing
	kill((-1 * getpgrp()), SIGUSR1);
}

/**
 * using with dup2, pipes and exec
 *
 */
void monitorWord(char *word, char *filename){
	//TODO
	sleep(60); //just for testing
}

/*
 * Ending program.
 */
void killAll(__pid_t pgids[], size_t size){
//TODO
}

