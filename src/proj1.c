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

int main(int argc, char *argv[]) {

	int run_time;
	__pid_t pid, pgr_id[(argc-3)];


	if(argc<4){
		puts("Invalid input.\nInput example: ./monitor time word filename1 filename2 ... filenameN\n");
		return -1;
	}

	run_time = atoi(argv[1]);
	printf("run_time = %d\n", run_time);  //for testing

	int i = 3;
	for(; i<argc; i++){
		int tmp = i -3;

		pid = fork();
		if(pid == 0){
			//word control
			monitorWord(argv[2], argv[i]);
			return 0;
		}
		else{
			//File existence verification
			__pid_t tmp_pid = pid;
			pgr_id[tmp]= pid;
			setpgid(pid, 0);


			if((pid = fork())==0){
				monitorAux(argv[i]);
				return 0;
			}
			else{
				setpgid(pid, tmp_pid);
				pgr_id[tmp]= tmp_pid;
			}
		}

	}



	return 0;
}


/*
 * check if files still exist every 5sec
 */
void monitorAux(char *file_name){
	//TODO
	sleep(1);	//just for testing
	printf("I'm monitor_aux, my pid is: %d\nMy pgid is: %d\n", (int) getpid(), (int) getpgrp()); //testing
	int fd;
	while(1){
		sleep(5);
		fd = open(file_name, O_RDONLY);
		if(fd== -1)
			break;
		else
			close(fd);
	}
	printf("File %s does not exist and will not be tracked\n", file_name);
	kill((-1 * getpgrp()), SIGUSR1);
}

/**
 * playing with dup2, pipes and exec
 *
 */
void monitorWord(char *word, char *filename){
	//TODO
	sleep(60); //just for testing
}

void killAll(__pid_t pgids[], size_t size){

}

