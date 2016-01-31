#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	int sockfd, tcp_socket, addrlen, n;
	struct sockaddr_in addr;
	char buf[512];

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[1]));
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(tcp_socket, (const struct sockaddr_in*)&addr, sizeof(struct sockaddr_in));
	listen(tcp_socket, 0);
	addrlen = sizeof(struct sockaddr);
	while(1)
	{
		sockfd = accept(tcp_socket, (struct sockaddr*)&addr, &addrlen);
		memset(buf, 0, sizeof(buf));	
		while(( n = read(sockfd, buf, sizeof(buf))) > 0)
		{
			printf("%s:%d", buf, strlen(buf));
		}
		printf("%d\n", sockfd);
	}
}


