#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/msg.h>

#define TIMER_MSG "Alarm received, killing processes, freeing memory\n"

FILE *toLogFile;

/*Initialize Shared Mem Keys*/
key_t MessageQkey = 640640;
key_t Clockkey = 464064;
key_t Prockey = 444767;

/*Initialize global vars*/
int spare = 0;

int * clock; //stores value of clock
int shm_clock = 0; // stores shmid of clock

int * maxproc; //stores value of maxproc
int shm_maxproc =0; //stores shmid of maxproc

int msgqid =0; //stores shmid/msgid of message queue

int timeout = 0;
char * logfile;
static volatile sig_atomic_t doneflag = 0;


/*ARGSUSED*/
static void setdoneflag(int signo){
	doneflag = 1;
}

struct sigaction act;

struct PCB {//Process Control Block
	pid_t procPID;
	long int timeQuantum;
	long int systemTime;
	int blocked; //1 if blocked, 0 if not blocked
	struct RCB * resources[10];
};

struct RCB { //Resource Control Block
	int used;
	int available;
	int share;
};


/*Function Protos*/
void createShm();
void releaseResources();
void printHelp();
