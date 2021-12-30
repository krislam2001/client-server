#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE 100

// Check operator
int is_operator(char item)
{
	if (item == '+' || item == '-' || item == '*' || item == '/')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// Check precedence
int precedence(char item)
{
	if (item == '*' || item == '/')
	{
		return 2;
	}
	else if (item == '+' || item == '-')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// Convert Infix to Postfix
void InfixToPostfix(char infix[], char postfix[], char stack[], char errorFormula[])
{
	int i = 0;  //index for infix
	int j = 0;  //index for stack
	int k = -1;  //index for postfix
	char item;
	
	stack[j] = '(';
	strcat(infix, ")");
	
	while (infix[i] != '\0')
	{
		item = infix[i];
		
		if (item == '(')
		{
			j++;
			stack[j] = item;
		}
		
		else if (isdigit(item))
		{
			k++;
			postfix[k] = item;
		}
		
		else if (is_operator(item) == 1)
		{
			if (precedence(stack[j]) > precedence(item))
			{
				k++;
				postfix[k] = stack[j];
				stack[j] = item;
			}
			else
			{
				j++;
				stack[j] = item;
			}
		}
		
		else if (item == ')')
		{
			while (stack[j] != '(')
			{
				k++;
				postfix[k] = stack[j];
				j--;
			}
			j--;
		}
		
		else
		{
			errorFormula[0] = 'Y';
		}
		
		i++;
	}
}

// Calculate postfix
int calcPostfix(char postfix[], int stackCalc[], char errorDivide0[])
{
	int i;  // index for postfix
	int j = -1;  // index for stack
	char item;
	int val;
	int a, b;
	int result;
	
	for (i = 0; postfix[i] != '\0'; i++)
	{
		item = postfix[i];
		if (isdigit(item))
		{
			j++;
			stackCalc[j] = item - '0';
		}
		else if (item == '+' || item == '-' || item == '*' || item == '/')
		{
			a = stackCalc[j];
			j--;
			b = stackCalc[j];
			j--;
			
			switch (item)
			{
				case '*':
				val = b * a;
				break;
				
				case '/':
				if (a == 0)
				{
					errorDivide0[0] = 'Y';
					return 0;
				}
				val = b / a;
				break;
				
				case '+':
				val = b + a;
				break;
				
				case '-':
				val = b - a;
			}
			
			j++;
			stackCalc[j] = val;
		}
	}
	
	result = stackCalc[j];
	return result;
}

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	char buffer[1024] = {0};
	//char* hello = "Hello";
	
	/* Create socket */
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Enter network address */
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("192.168.179.130"); //Enter IP of this comp
	server_address.sin_port = htons(9734);
	server_len = sizeof(server_address);
	
	/* Bind socket */
	bind(server_sockfd, (struct sockaddr*) &server_address, server_len);
	
	/* Listen */
	listen(server_sockfd, 5);
	
	int c = 1;
	
	while(c)
	{
		/* Wait for client */
		printf("server waiting... \n");
	
		/* Accept client */
		client_sockfd = accept(server_sockfd, (struct sockaddr*) &client_address, (socklen_t*)&server_len);
	
		/* Read */
		char postfix[SIZE];
		char stack[SIZE];
		int stackCalc[SIZE];
		int result;
		char errorFormula[1];
		errorFormula[0] = 'N';
		char errorDivide0[1];
		errorDivide0[0] = 'N';
		char errorSignal = 'Y';
		char validSignal = 'N';
		
		// Get formula from client
		read(client_sockfd, buffer, 1024);
		printf("Formula: %s", buffer);
		printf("\n");
		
		// Convert to postfix version
		InfixToPostfix(buffer, postfix, stack, errorFormula);
		printf("Error formula: %c", errorFormula[0]);
		printf("\n");
		
		if (errorFormula[0] == 'Y')
		{
			write(client_sockfd, &errorSignal, 1);
			
			close(client_sockfd);
		}
		else
		{
			write(client_sockfd, &validSignal, 1);
			read(client_sockfd, buffer, 1024);
			printf("Postfix version: %s", postfix);
			printf("\n");
		
			// Calculate postfix
			result = calcPostfix(postfix, stackCalc, errorDivide0);
			printf("Error: %c", errorDivide0[0]);
			printf("\n");
			if (errorDivide0[0] == 'Y')
			{
				write(client_sockfd, &errorSignal, 1);
			
				close(client_sockfd);
			}
			else
			{
				write(client_sockfd, &validSignal, 1);
				printf("Result: %d", result);
				printf("\n");
				read(client_sockfd, buffer, 1024);
				write(client_sockfd, &result, 4);
	
				close(client_sockfd);	
			}
		}
		// c = 0;
	}
	
	close(server_sockfd);
	
	return 0;
}
