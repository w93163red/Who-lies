/*
	
*/


#include "server.h"

int main()
{
	void *t; 	//an useless variable.
	pthread_t pt_reg, pt_conn, pt_ctrl;

	initial();
	pthread_create(&pt_conn, NULL, &connecter, NULL);
	pthread_create(&pt_ctrl, NULL, &controler, NULL);

	pthread_join(pt_conn, NULL);
	pthread_join(pt_ctrl, NULL);
	return 0;
}

void initial()		//Initialize message queues.
{
	msqid = msgget(MSGKEY, 0777|IPC_CREAT);
	con_msqid = msgget(CON_NUM_MSGKEY, 0777|IPC_CREAT);
	msgctl(msqid, IPC_RMID, NULL);
	msgctl(con_msqid, IPC_RMID, NULL);	//if exists, destroy it.

	if((msqid = msgget(MSGKEY, 0777|IPC_CREAT)) == -1)
		perror("msqid msgget:");
	if((con_msqid = msgget(CON_NUM_MSGKEY, 0777|IPC_CREAT)) == -1)
		perror("con_msqid msgget:");

	poker_order = (char *)malloc(52);
	memset(poker_order, '\0', sizeof(poker_order));
}

void* connecter(void *t)	//multi-processes 
{
	int sockfd, tcp_socket, addrlen, n, fork_fd;
	struct sockaddr_in addr;
	char buf[512];
	struct msgbuf con_msg, msg;

	signal(SIGCHLD, SIG_IGN); 
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORTNUM);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(tcp_socket, (const struct sockaddr*)&addr, sizeof(addr));
	listen(tcp_socket, 1);
	addrlen = sizeof(struct sockaddr);
	while(1)
	{
		sockfd = accept(tcp_socket, (struct sockaddr*)&addr, &addrlen);
		fork_fd = fork();
		if (fork_fd == 0)
		{
			con_msg.mtype = sockfd;
			if ((msgsnd(con_msqid, (void *)&con_msg, sizeof(con_msg.mtext), 0)) == -1)
				perror("con_msqid msgsnd:");
			while((n = read(sockfd, buf, sizeof(buf)) > 0))
			{
				msg.mtype = sockfd;
				strcpy(msg.mtext, buf);
				printf("LOG: connecter put msg(%d------%s) into queue\n", msg.mtype, msg.mtext);
				if ((msgsnd(msqid, (void *)&msg, sizeof(msg.mtext), 0)) == -1)
					perror("msqid msgsnd:");
			}
			close(sockfd);
		}
	}
}

void regist()	//Recording socket fds
{
	int n = 0;
	int i;
	struct msgbuf id;
	do
	{
		if((msgrcv(con_msqid, (void *) &id, sizeof(id.mtext), 0, 0)) == -1)
			perror("con_msqid msgrcv:");	
		connecter_fd[n] = id.mtype;
		printf("%d %d %d\n", n != MAXUSERNUM, n, connecter_fd[n]);
		n = n+1;
	}while(n != MAXUSERNUM);
	printf("LOG: ALL USER READY\n");
}

void poker_gen()
{
	int f[55]={0};
	char pokers[52];
	int i=0,poker;

	memset(pokers, '\0', sizeof(pokers));
	srand((int)time(0));
	while(i < 52)
	{
		poker = random(52);
		if(!f[poker])
		{
			f[poker] = 1;
			pokers[i++] = pokertochar(poker);	
		}
	}
	strcpy(poker_order, pokers);
	printf("LOG: pokers = %s\n", pokers);
}

char pokertochar(int poker)
{
	switch(poker % 13)
	{
		case 10: return '+';
		case 11: return 'J';
		case 12: return 'Q';
		case 0 : return 'K';
		case 1 : return 'A';	
		default: return '0' + poker % 13;
	}
}

void* controler(void *t)	//control the game's order.
{
	int i, n, j;
	struct msgbuf msg;
	char buf[512], current_user_put_pokers[512], current_round_pokers[512], current_user_claim_pokers[512];
	regist();
	poker_gen();
	poker_sender();
	while(1)
	{
		memset(current_round_pokers, '\0', sizeof(current_round_pokers));
		for(i = 0; i < MAXUSERNUM; i++)
		{
			printf("LOG: controler SEND \"TURN\" TO %d\n", connecter_fd[i]);
			write(connecter_fd[i], "TURN", strlen("TURN"));

			//read the real pokers
			msg = readmsg();	
			strcpy(buf, msg.mtext);
			printf("LOG: %d-----%s\n", msg.mtype, msg.mtext);
			sscanf(buf, "%*s%s", current_user_put_pokers); 		
			printf("LOG: %d CURRENT_USER_PUT_POKERS: %s\n", connecter_fd[i], current_user_put_pokers);
			strcat(current_round_pokers, current_user_put_pokers);
			printf("LOG: %d CURRENT_ROUND_POKERS: %s\n", connecter_fd[i], current_round_pokers);

			//read the claim pokers
			msg = readmsg();
			strcpy(buf, msg.mtext);	
			printf("LOG: %d-----%s\n", msg.mtype, msg.mtext);
			sscanf(buf, "%*s%s", current_user_claim_pokers); 		
			printf("LOG: %d CURRENT_USER_CLAIM_POKERS: %s\n", connecter_fd[i], current_user_claim_pokers);
			broadcast(connecter_fd[i], current_user_claim_pokers);

			//Judge Part
			judgement(i, current_round_pokers, current_user_put_pokers, current_user_claim_pokers);

			//Add pokers part, under consideration, add this function further.
			

			printf("%s\n", msg.mtext);
			if (!strcmp(msg.mtext, "OVER"))
			{
				printf("GAME OVER, THE WINNER IS %d\n", msg.mtype);
				break;
			}
		}
	}
}

void poker_sender()
{
	int i,pokernum,totpoker=52;
	char poker[30];
	for (i = 0; i < MAXUSERNUM; i++)
	{
		memset(poker, '\0', sizeof(poker));
		strcpy(poker, "FORCE ");
		pokernum = totpoker / (MAXUSERNUM - i);
		strncat(poker, poker_order, pokernum);
		totpoker -= pokernum;
		poker_order += pokernum;
		printf("LOG: poker_sender SEND \"%s\"  TO %d\n", poker, connecter_fd[i]);
		write(connecter_fd[i], poker, sizeof(poker));	
	}
}

void broadcast(int n, char buf[])
{
	int i;
	char order[512] = "BROADCAST ";

	order[10] = '0' + n;	// WILL FIX THIS BUG FURTHER. A DANGEROUS WAY.
	order[11] = ' ';
	strcat(order, buf);
	order[strlen(order)] = ' ';
	order[strlen(order)+1] = '\0';
	for (i = 0; i < MAXUSERNUM; i++)
	{
		printf("LOG: broadcast SEND \"%s\" TO %d\n", order, connecter_fd[i]);
		write(connecter_fd[i], order, sizeof(order)); 
	}

}

struct msgbuf readmsg()
{
	struct msgbuf msg;
	if((msgrcv(msqid, &msg, sizeof(msg.mtext), 0, 0)) == -1)
	{
		perror("controler msgrcv:");
	}			
	printf("LOG: readmsg-----%d %s\n", msg.mtype, msg.mtext);
	return msg;
}

void judgement(int start, char* current_round_pokers, char* current_user_put_pokers, char* current_user_claim_pokers)
{
	int i;
	char buf[512], user_judge[512];
	for(i = 1; i < MAXUSERNUM; i++)
	{
		write(connecter_fd[(i+start)%MAXUSERNUM], "JUDGE", strlen("JUDGE"));
		msg = readmsg();
		strcpy(buf, msg.mtext);
		printf("LOG: %d-----%s\n", msg.mtype, msg.mtext);
		sscanf(buf, "%*s%s", user_judge);
		broadcast(connecter_fd[(i+start)%MAXUSERNUM], user_judge);
		if (!strcmp(user_judge, "FALSE"))
		{
			sprintf(buf, "%s %s ", "FORCE", current_round_pokers);	
			printf("%d\n", strcmp(current_user_put_pokers, current_user_claim_pokers));
			if(strcmp(current_user_put_pokers, current_user_claim_pokers) != 0)
				write(connecter_fd[start], buf, sizeof(buf));
			else
				write(connecter_fd[(i+start)%MAXUSERNUM], buf, sizeof(buf));
		}
	}
}
