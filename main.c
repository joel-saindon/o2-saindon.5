#include "info.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int main (int argc, char * argv[]){

	int opt;
	while((opt = getopt(argc,argv, "hs:")) != -1){
		switch (opt){
			case 'h'://display help message, quit
				printHelp();
				return -1;
			case 's'://establish max processes to be run at once
				printf("s option, args %d \n", argc);
				//printf("optarg: %s\n", optarg);
				shm_maxproc = shmget(Prockey, sizeof(int), IPC_CREAT | 0666);
				maxproc = (int *)shmat(shm_maxproc,NULL,0);
				*maxproc = atoi(argv[2]);
				//maxproc = &spare;
		/*		int shm_maxproc = shmget(shmkey3, sizeof(int), IPC_CREAT | 0666)
				if(shm_maxproc < 0){
					perror("maxproc shmget fail");
					exit(1);
				}
		*/
				break;
			/*case '?':
				printf("Usage: \n");
				return -1;
			*/
			default:
				break;	
		}
	}
	
	if(argc < 2 || argc == 2 ){
					printf("No correct values detected, or incorrect format: using defaults.\n");
					printf("DEBUG: before setting maxproc");
					spare = 5;
					maxproc = &spare;
					printf("DEBUG: after setting maxproc");
					printf("Maxproc now %d", *maxproc);
				}

	printf("Hello World\n");
	printf("maxproc: %d\n", *maxproc);
	
	/*
	int shm_maxproc = shmget(shmkey3, sizeof(int), IPC_CREAT | 0666)
	if(shm_maxproc < 0){
		perror("maxproc shmget fail");
		exit(1);
	}

	*maxproc = shmat(shm_maxproc, NULL, 0);
	*/
	int status;
	pid_t childpid = 0;
	pid_t waitreturn;
	int i = 0;

	for(i=0; i< 1; i++){
		if((childpid = fork()) < 0){
			perror("fork");
			return -1;
		}
		else if (childpid == 0){
			printf("child created %d\n", getpid());
			execl("./oss", "./oss", NULL);
			exit(0);
		}
		else{
			(void)waitpid(childpid, &status, 0);
		}
	}

return 0;
}

void printHelp(){
	printf("---------------HELP TEXT-----------------------\n");
	printf("Options: \n");
	printf("./proj5 -s x, where x is number of max procs\n");
	printf("./proj5 , runs with defaults that are hard coded in\n");
	printf("./proj5 -h, runs help file...duh\n");
	printf("---------------END HELP------------------------\n");
}
