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

void sig_handler(int signo)
{
	//printf("Handled\n");
}



int main(int argc, char *argv[]) {

	int run_time;
	__pid_t pid, pgr_id[(argc-3)];

	struct sigaction action;
	action.sa_handler = sig_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	if (sigaction(SIGUSR1, &action, NULL) < 0)
	{
		fprintf(stderr,"Unable to install SIGNAL handler\n");
		exit(1);
	}


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

		if(DEBUG)
			printf("\nprocess nr: %d\n", tmp);

		pid = fork();
		if(pid == 0)
		{
			//word control
			monitorWord(argv[2], argv[i]);
			return 0;
		}
		else
		{
			__pid_t tmp_pid = pid;
			pgr_id[tmp]= pid;
			setpgid(pid, 0); //child pgrid is the sames as his pid

			//creating monitor_aux for the file and process
			if((pid = fork())==0)
			{
				monitorAux(argv[i]);
				return 0;
			}
			else
			{
				setpgid(pid, tmp_pid);	//monitor_aux pgrid to the same of monitor_word
				pgr_id[tmp]= tmp_pid;
			}
		}

	}

	while(( run_time = sleep(run_time)) != 0);

	killAll(pgr_id, (argc -3));

	return 0;
}


/*
 * check if files still exist every 5sec
 */
void monitorAux(char *filename){
	//TODO
	sleep(1);

	if(DEBUG)
	{
		printf("I'm a monitor_aux, my pid is: %d and my pgid is: %d\n", (int) getpid(), (int) getpgrp()); //testing
		printf("%d: I'm watching %s\n", (int) getpid(), filename); //testing
	}

	int fd;
	while(1){
		sleep(5);
		fd = open(filename, O_RDONLY);
		if(fd== -1)
			break;
		else
			close(fd);
	}

	printf("File %s does not exist and will not be tracked\n", filename);

	kill((-1 * getpgrp()), SIGTERM);
}

/**
 * using with dup2, pipes and exec
 *
 */
void monitorWord(char *word, char *filename){
	//TODO
	int pipe1[2], pipe2[2];
	pipe(pipe1);
	__pid_t pid;

	if((pid=fork())==0)
	{
		close(pipe1[READ]);
		dup2(pipe1[WRITE], STDOUT_FILENO);
		execlp("tail", "tail", "-fn 1", filename, NULL);
	}
	else
	{
		setpgid(pid, getpid());
		pipe(pipe2);

		if((pid=fork())== 0)
		{
			close(pipe1[WRITE]);
			close(pipe2[READ]);
			dup2(pipe1[READ], STDIN_FILENO);
			dup2(pipe2[WRITE], STDOUT_FILENO);
			execlp("grep", "grep", "--line-buffered", word, NULL);
		}
		else
		{
			setpgid(pid, getpid());
		}
		sleep(60); //TODO
	}

	sleep(60); //just for testing
}

/*
 * Ending program.
 */
void killAll(__pid_t pgids[], size_t size){
	//TODO
	size_t i=0;
	for(; i<size; i++)
	{
		if(pgids[i]!= 0)
			kill((-1*pgids[i]), SIGUSR1);
	}
}
