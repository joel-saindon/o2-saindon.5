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
	char * oFile = "test.out";
	int i;
	int status;
	printf("from OSS\n");
	sleep(2);
	toLogFile = fopen(oFile, "w");	//overwrite old results each run
	if(toLogFile == 0){
		perror("Could not open file");
		exit(0);
	}
	fprintf(toLogFile, "Test test\n");

	createShm();// initialize shared memory segments
	
	printf("DEBUG\n");
	//printf("%d", spare);
	//printf("%d", *maxproc);
/*
	//fork user processes
	printf("maxproc before fork loop: %d\n", *maxproc);
	pid_t pids[*maxproc];
	for(i=0; i< *maxproc; i++){
		if((pids[i] = fork()) < 0){
			perror("fork");
			return 1;
		}
		else if(pids[i] == 0){ //if in forked process
			printf("user process created %ld\n", getpid());
			execl("./user", "./user", NULL);
			exit(0);
		}
		else{
			(void)waitpid(pids[i], &status, 0);
		}
	}
*/	
	releaseResources(); //release shared memory segments
	sleep(1);
	printf("oss done\n");
	fclose(toLogFile);

return 0;
}


void releaseResources(){
	printf("Cleaning up Resources\n");
	//detach clock from shared memory
	int clockRelease = shmdt(clock);
	if( clockRelease < 0){
		perror("shmdt fail on releasing clock var\n");
	}
	
	
	//delete/release shared memory 
	if(shmctl(shm_clock, IPC_RMID, NULL) == -1){
		perror("shmctl error on clock var\n");
	}
	
	//detach maxproc from shared memory
	int maxprocRelease = shmdt(maxproc);
	if(maxprocRelease < 0){
		perror("shmdt fail on releasing maxproc\n");
	}
	
	//release maxproc shared mem segment
	int maxprocDeleteError = shmctl(shm_maxproc, IPC_RMID, NULL);
	if(maxprocDeleteError < 0){
		perror("shmctl error on maxproc var \n");
	}
	//deallocate message queue
	int msgerror = msgctl(msgqid, IPC_RMID, NULL);
	if(msgerror == -1){
		perror("msg queue removal error\n");
		exit(1);
	} else 
		printf("msg queue deallocated\n");
	
}

void createShm(){
	//printf(toLogFile,"---creating memory for clock and message queue---");
	//get shared memory segment for clock
	shm_clock = shmget(Clockkey, sizeof(int), IPC_CREAT | 0666);
	if(shm_clock < 0){
		perror("shmget failed");
		exit(1);	
	}

	/*//get shared mem segment for maxproc
	shm_maxproc = shmget(Prockey, sizeof(int), IPC_CREAT | 0666);
	if(shm_maxproc < 0){
		perror("maxproc shmget failed");
		exit(1);
	}
	*maxproc = spare;
	*/
	//initialize message queue
	int msgflag = 666;
	
	msgqid = msgget(MessageQkey, IPC_CREAT | 0666);
	if(msgqid == -1){
		perror("mssget");
		exit(1);
	} else
		printf("msg queue created\n");
	
	//attach clock to shared memory
	clock = (int *)shmat(shm_clock,NULL,0);
	//printf(toLogFile, "clock initialized\n");
	//int * test = clock;
	//*test = *test + 1;
	
	//attach maxproc to shared memory
	maxproc = (int *)shmat(shm_maxproc,NULL,0);
	printf("maxproc attached\n");
	
}
