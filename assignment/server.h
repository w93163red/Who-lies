#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <time.h>
#define PORTNUM 10000
#define MSGKEY 75 
#define CON_NUM_MSGKEY 78
#define MAXUSERNUM 2
#define random(x) (rand()%x)

struct msgbuf
{
	int mtype;
	char mtext[120];
}msg;
static int connecter_fd[10]={0};
char *poker_order;
int msqid, con_msqid;

void *connecter(void *);
void *controler(void *);
void msg_send();
void initial();
void regist();
void poker_gen();
void broadcast(int, char buf[]);
int type_converter(char buf[]);
struct msgbuf readmsg();
char pokertochar(int );
void poker_sender();
void judgement(int, char*, char*, char*);
