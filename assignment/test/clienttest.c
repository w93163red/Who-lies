#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#define PORT 10000

using namespace std;

int main(int argc, char* argv[])
{
	int sockfd, tcp_socket, addrlen, n;
	struct sockaddr_in addr;
	char buf[512];

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	addrlen = sizeof(struct sockaddr_in);
	connect(tcp_socket, (struct sockaddr*)&addr, addrlen);
	read(tcp_socket, buf, sizeof(buf));
	printf("%s\n", buf);
	close(tcp_socket);	
}



