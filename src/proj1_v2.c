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

__pid_t *pgrids;
int sizeof_pgrids;

/*
 * handle terminated children and classifies the process group as dead, so the parent don't try to kill it again
 */
void sig_handler(int signo)
{

	if(DEBUG)
	{
		printf("sig_handler of pid:%d\n", getpid());
	}
}



int main(int argc, char *argv[]) {


	int runtime, alive_procs=(argc-3), status;
	sizeof_pgrids=alive_procs;
	__pid_t pid;

	struct sigaction action;
	action.sa_handler = sig_handler;
	sigemptyset(&action.sa_mask);
	//action.sa_flags = SA_NOCLDWAIT; //v1
	action.sa_flags = 0; //v2

	if (sigaction(SIGCHLD, &action, NULL) < 0)
	{
		fprintf(stderr,"Unable to install SIGNAL handler\n");
		exit(1);
	}
	status = 3;
	for(; status<argc; status++){
		int fd= open(argv[status], O_RDONLY);
		if(fd==-1){
			fprintf(stderr, "One or more files given not found\n");
			exit(1);
		}
		close(fd);
	}
	pgrids=malloc(sizeof(__pid_t) * alive_procs);


	if(argc<4)
	{
		puts("Invalid input.\nInput example: ./monitor time word filename1 filename2 ... filenameN\n");
		return -1;
	}

	runtime = atoi(argv[1]);
	int i = 3;
	for(; i<argc; i++)
	{
		int tmp = i -3;

		if((pid= fork()) == 0)
		{
			//word control
			monitorWord(argv[2], argv[i]);
			return 0;
		}
		else
		{
			pgrids[tmp]= pid;
			setpgid(pid, pid); //child pgrid is the same as his pid
		}

	}



	//creating monitor_aux for the file and process
	if((pid = fork())==0)
	{
		monitorExistence(argv, pgrids, argc);
		return 0;
	}
	if(DEBUG)
		printf("alive processes:%d\n", alive_procs);
	while(alive_procs>0)
	{	//v2
		__pid_t pid_tmp= waitpid(-1, &status, WNOHANG);

		runtime=sleep(runtime);
		if(runtime==0)
			break;

		if(pid_tmp>0){
			printf("pid_tmp:%d\n", pid_tmp);
			alive_procs--;
			doNotFollow(pid_tmp, pgrids, sizeof_pgrids);
		}
	}
	killAll(pgrids, &sizeof_pgrids, pid); //v2
	return 0;
}


/*
 * check if files being monitored still exist every 5sec
 * filenames given from argv[3] to argv[argc-1]
 * if the file does not exist anymore, the process group monitoring its words is killed
 */
void monitorExistence(char *argv[], __pid_t *pgrids, int argc){

	if(DEBUG)
		printf("I am the existence monitor. My pid: %d. My ppid: %d\n", getpid(), getppid());

	while(1){
		sleep(5);
		int i=3;
		for(; i<argc; i++)	//trying to open the files
		{
			int tmp = i-3;	//tmp is the matching pid index at pgrid
			if(pgrids[tmp] != 0)
			{
				if(DEBUG)
					printf("Verifying existence of %s, that belongs with %d group\n", argv[i], pgrids[tmp]);

				if(checkFileExistence(argv[i]) == -1){
					killOneProcessGroup(pgrids[tmp]);
					pgrids[tmp]=0;
				}
			}
		}
	}
}


/**
 * Creates 2 children, one running tail and one grep, connecting them with a pipe, and connects itself
 * with grep, so it can catch every phrase with the word you want to find in the file filename.
 */
void monitorWord(char *word, char *filename){
	//sleep(TIME_BEFORE_LAUNCH);
	int pipe1[2], pipe2[2];
	pipe(pipe1);
	__pid_t pid;

	if((pid=fork())==0)
	{
		//tail
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
			//grep
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
			//console output
			if(DEBUG)
				whoAmI(filename, "grep output to console");
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
void killAll(__pid_t *pgids, int *alive_procs, __pid_t file_checker){
	int i=0;
	int kill_return;
	if(DEBUG)
		printf("This is killAll\n");
	for(; i< (*alive_procs); i++)
	{
		if(DEBUG)
			printf("pgid[%d]: %d\n", i, pgids[i]);
		if(pgids[i]!= 0)
			if((kill_return=kill((-1*pgids[i]), SIGUSR1)) == -1)
				printf("error killing group %d: %s\n", pgids[i], strerror(errno));

	}
	if((kill_return=kill(file_checker, SIGUSR1)) == -1)
		printf("error killing %d: %s\n", file_checker, strerror(errno));
}


/*
 * For Debug purposes only.
 */
void whoAmI(char *filename, char *whatIDo){
	printf("I'm %d, son of %d, of the group %d. I do %s of %s\n",getpid(), getppid(), getpgrp(), whatIDo, filename);
}


/*
 * sets to 0 matching pid in the array of pids, so we can avoid trying to kill an inexistent process
 */
void doNotFollow(__pid_t pgrid, __pid_t *pgrids, int size){
	int i= 0;
	//printf("pgr to kill: %d, pgrids:%d\n", pgrid, pgrids[0]);
	while(pgrids[i] != pgrid && i< sizeof_pgrids){
		i++;
	}
	if(i<sizeof_pgrids){
		pgrids[i]=0;
		if(DEBUG)
			printf("i= %d: pgrid removed: %d\n", i, pgrids[i]);
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
		usleep(50);
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
