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

/*
 * handle terminated children
 */
void sig_handler(int signo)
{
	if(DEBUG)
		printf("handled a child\n");
}


int main(int argc, char *argv[]) {

	int run_time, status;
	__pid_t pid, pgr_id[(argc-3)];

	struct sigaction action;
	action.sa_handler = sig_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags =  SA_NOCLDWAIT;

	if (sigaction(SIGCHLD, &action, NULL) < 0)
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

		if((pid= fork()) == 0)
		{
			//word control
			monitorWord(argv[2], argv[i]);
			return 0;
		}
		else
		{
			pgr_id[tmp]= pid;
			setpgid(pid, pid); //child pgrid is the sames as his pid
			//waitpid(-pid, &status, WNOHANG);
		}

	}
	waitForChildren(pgr_id, status, (argc-3));

	//creating monitor_aux for the file and process
	if((pid = fork())==0)
	{
		monitorExistence(argv, pgr_id, argc);
		return 0;
	}
	waitpid(pid, &status, WNOHANG);

	while(( run_time = sleep(run_time)) != 0);

	killAll(pgr_id, (argc -3), pid);


	return 0;
}

/*
 * check if files still exist every 5sec
 */
void monitorExistence(char *argv[], __pid_t pgrid[], int argc){
	//TODO
	sleep(1);

	if(DEBUG)
		printf("I am the existence monitor. My pid: %d. My ppid: %d\n", getpid(), getppid());

	int fd;
	int i;
	while(1){
		sleep(5);
		i=3;
		for(; i<argc; i++)
		{
			int tmp = i-3;
			if(pgrid[tmp] != 0)
			{

				fd = open(argv[i], O_RDONLY);
				if(fd== -1)
				{
					printf("File %s does not exist and will not be tracked\n", argv[i]);
					kill(-pgrid[tmp], SIGUSR1);
					pgrid[tmp]=0;
				}
				else{
					close(fd);
					if(DEBUG)
						printf("Verified existence of %s, that belongs with %d group\n", argv[i], pgrid[tmp]);
				}
			}

		}
	}
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
		if(DEBUG)
			whoAmI(filename, "tail");
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
			if(DEBUG)
				whoAmI(filename, "grep");
			close(pipe1[WRITE]);
			close(pipe2[READ]);
			dup2(pipe1[READ], STDIN_FILENO);
			dup2(pipe2[WRITE], STDOUT_FILENO);
			execlp("grep", "grep", "--line-buffered", word, NULL);
		}
		else
		{
			if(DEBUG)
				whoAmI(filename, "communication");
			setpgid(pid, getpid());
			close(pipe2[WRITE]);

			time_t t = time(NULL);
			struct tm tm = *localtime(&t);
			char receive[MAX_SIZE];
			int read_bytes= 0;

			while((read_bytes= read(pipe2[READ], receive, MAX_SIZE))!= -1 || read_bytes!= 0)
			{
				t= time(NULL);
				tm = *localtime(&t);
				receive[read_bytes]='\0';
				printf("%4d-%02d-%02dT%02d:%02d:%02d - \"%s\" - %s", tm.tm_year + 1900, tm.tm_mon, tm.tm_mday,
						tm.tm_hour, tm.tm_min, tm.tm_sec, filename, receive);
			}
		}
	}
}

/*
 * Kills all processes
 */
void killAll(__pid_t pgids[], size_t size, __pid_t file_checker){
	//TODO
	size_t i=0;
	for(; i<size; i++)
	{
		if(pgids[i]!= 0)
			kill((-1*pgids[i]), SIGUSR1);
	}
	kill(file_checker, SIGUSR1);
}

void waitForChildren(__pid_t pgids[], int status, size_t size){
	int i =0;
	for(; i<size; i++)
		waitpid(pgids[i],  &status, WNOHANG);
}

/*
 * Debug purposes
 */
void whoAmI(char *filename, char *whatIDo){
	printf("I'm %d, son of %d, of the group %d. I do %s of %s\n",getpid(), getppid(), getpgrp(), whatIDo, filename);
}
