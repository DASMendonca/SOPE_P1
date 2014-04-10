/*
============================================================================
Name : proj1.c
Author : Daniel Mendonça
Version :
Copyright : Your copyright notice
Description : Project 1, SOPE
============================================================================
 */

#include "proj1.h"


int buried_children=0;

/*
 * handle terminated children
 */
void sig_handler(int signo)
{
	if(DEBUG)
		printf("I, %d, buried a child has died\n", getpid());
	buried_children++;
}


int main(int argc, char *argv[]) {


	int runtime, alive_procs=(argc-3);
	__pid_t pid, pgids[alive_procs];

	struct sigaction action;
	action.sa_handler = sig_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_NOCLDWAIT;

	if (sigaction(SIGCHLD, &action, NULL) < 0)
	{
		fprintf(stderr,"Unable to install SIGNAL handler\n");
		exit(1);
	}

	validator(argc, argv, &runtime);

	monitorWordCreator(argv, argc, pgids, &pid);

	//creating monitor_aux for the file and process
	iFork(&pid);
	if(pid==-1){
		perror("Error. Could not monitor files\n");
		killAll(pgids, &alive_procs, pid);
		exit(-1);
	}
	if(pid==0)
	{
		monitorExistence(argv, pgids, argc);
		return 0;
	}

	if(DEBUG)
		printf("alive_procs: %d\n", alive_procs);

	while(( runtime = sleep(runtime)) != 0 && alive_procs>buried_children){
		if(DEBUG)
				printf("alive_procs: %d buried_children:%d\n", alive_procs, buried_children);
	}

	alive_procs = argc -3;
	killAll(pgids, &alive_procs, pid);

	return 0;
}


/*
 * check if files still exist every 5sec
 */
void monitorExistence(char *argv[], __pid_t *pgids, int argc){

	if(DEBUG)
		printf("I am the existence monitor. My pid: %d. My ppid: %d\n", getpid(), getppid());

	while(1){
		int i=3;
		sleep(5);
		for(; i<argc; i++)
		{
			int tmp = i-3;
			if(DEBUG)
				printf("This is existence monitor, pgrid[%d] = %d\n", tmp, pgids[tmp]);
			if(pgids[tmp] != 0)
			{
				int return_val= checkFileExistence(argv[i]);
				if(return_val==-1){
					killOneProcessGroup(pgids[tmp]);
					pgids[tmp]=0;
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
				//tm_mon january is 0 december is 11
				printf("%4d-%02d-%02dT%02d:%02d:%02d - %s - %s", tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday,
						tm.tm_hour, tm.tm_min, tm.tm_sec, filename, receive);
			}
		}
	}
}


/*
 * Kills all processes
 */
void killAll(__pid_t *pgids, int *nr_created_procs, __pid_t file_checker){
	int i=0;
	int kill_return;
	if(DEBUG)
		printf("This is killAll\n");

	for(; i< (*nr_created_procs); i++)
	{
		if(DEBUG)
			printf("pgid[%d]: %d\n", i, pgids[i]);

		if(pgids[i]!= 0)
			kill_return=kill((-1*pgids[i]), SIGUSR1);
		if(DEBUG){
			if(kill_return==-1)
			printf("error killing group %d: %s\n", pgids[i], strerror(errno));
		}
	}
	kill_return=kill(file_checker, SIGUSR1);
	if(DEBUG){
		if(kill_return==-1)
		printf("error killing fileChecker %d: %s\n", file_checker, strerror(errno));
	}
}


/*
 * Debug purpose
 */
void whoAmI(char *filename, char *whatIDo){
	printf("I'm %d, son of %d, of the group %d. I do %s of %s\n",getpid(), getppid(), getpgrp(), whatIDo, filename);
}


/*
 * sets to 0 matching pid in the array of pids to be killed at the end of the program
 */
void doNotFollow(__pid_t pgrid, __pid_t *pgrids){
	int i= 0;
	printf("pgr to kill: %d, pgrids:%d\n", pgrid, pgrids[0]);
	while(pgrids[i] != pgrid){
		i++;
	}
	pgrids[i]=0;
	if(DEBUG)
		printf("i= %d: pgrid removed: %d\n", i, pgrids[i]);
}


/*
 * creates all monitor_aux, the process that looks for the word in the file
 */
void monitorWordCreator(char *argv[], int argc, __pid_t *pgids, __pid_t *pid){
	int i = 3;

	for(; i<argc; i++){
		int tmp = i -3;

		iFork(pid);
		if((*pid)==0)
		{
			//word control
			monitorWord(argv[WORD_INDEX], argv[i]);
		}
		else if((*pid)==-1){
			pgids[tmp]=0;
		}
		else
		{
			pgids[tmp]= (*pid);
			setpgid((*pid), pgids[tmp]); //child pgrid is the sames as his pid
		}

	}
}


//checks if user gave a good input
void validator(int argc, char* argv[], int *runtime){
	//TODO
	if(argc<4){
		puts("Invalid number of args.\nInput example: ./monitor time_to_live word filename1 filename2 ... filenameN\n");
		exit(-1);
	}

	(*runtime) = atoi(argv[1]);
	if((*runtime)<=0){
		puts("The time_to_live you set is invalid. Must be >0\n");
		exit(-1);
	}
}


/*
 * Try to open with readonly permission the filename. Return -1 if it fails.
 */
int checkFileExistence(char *filename){
	int fd;

	fd = open(filename, O_RDONLY);
	if(fd== -1)
	{
		printf("%s: %s\n", filename, strerror(errno));
		return -1;
	}
	close(fd);
	return 0;
}


//kill a process group
void killOneProcessGroup(__pid_t pgid){
	if(kill(-pgid, SIGUSR1)== -1){
		printf("%s\n", strerror(errno));
	}
}

void iFork(__pid_t *pid){
	(*pid)=fork();
	if((*pid) < 1)
		printf("%s\n", strerror(errno));
}

