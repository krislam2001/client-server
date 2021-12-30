#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE 100

int main()
{
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	char buffer[1024] = {0};
	char signal;
	
	int result_received;
	
	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Create address*/
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1"); //Enter IP of this comp
	address.sin_port = htons(9734);
	len = sizeof(address);
	
	/* Connect */
	result = connect(sockfd, (struct sockaddr*) &address, len);
	if (result == -1)
	{
		perror("Oops: client problem");
		return 1;
	}
	
	/* Read, write, close */
	char string[SIZE];
	printf("\n");
	
	// Enter formula
	printf("Enter formula: ");
	scanf("%s", string);
	
	// Send to server
	send(sockfd, string, strlen(string), 0);
	
	// Read signal
	read(sockfd, &signal, 1);
	if (signal == 'Y')
	{
		printf("FORMULA INVALID !");
		printf("\n");
		close(sockfd);
	}
	else
	{
		send(sockfd, string, strlen(string), 0);
		read(sockfd, &signal, 1);
		
		if (signal == 'Y')
		{
			printf("DIVIDE BY 0 !");
			printf("\n");
			close(sockfd);
		}
		else
		{
			send(sockfd, string, strlen(string), 0);
			read(sockfd, &result_received, 4);
			printf("The result is: %d\n", result_received);
			close(sockfd);
		}
	}
	
	return 0;
}