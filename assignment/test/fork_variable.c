#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#define MSGKEY 70
struct msgbuf
{
	long mtype;
	char mtext[1024];
};
int msgqid, nbytes;
int a = -1;

void thread(void *a)
{
	int n, error;
	signal(SIGCHLD, SIG_IGN);
	msgqid = msgget(MSGKEY, 0777 | IPC_CREAT);
	n = fork();
	if (n == 0)
	{
		struct msgbuf msg;
		a = 1;
		msg.mtype = a;
		printf("msgqid = %d\n", msgqid);
		strcpy(msg.mtext, "hello world");
		printf("msg.mtype = %d\n", msg.mtype);
		if((error = msgsnd(msgqid, &msg, 512, 0))== -1)
			perror("msgsnd");
		printf("child a: %d\n", a);
	}
	else
	{
		printf("msgqid = %d\n", msgqid);
		struct msgbuf parent_msg;
		printf("before msg a: %d\n", a);
		if ((error = msgrcv(msgqid, &parent_msg, 512, 0, 0)) == -1)
			perror("msgrcv");
		a = parent_msg.mtype;
		msgctl(msgqid, IPC_RMID, NULL);
		a = a+2;
		printf("after parent a: %d\n", a);
	}

}


int main()
{
	pthread_t pt1;

	pthread_create(&pt1, NULL, &thread, NULL);

	pthread_join(pt1, NULL);
	return 0;

}
