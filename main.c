#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "mystring.h"
#include "stringarray.h"

int server;

void interrupt(int x)
{
	if (x == SIGINT)
	{
		close(server);
		exit(0);
	}
}

void sender(int client, char *fileName)
{
	String str = initString();
	FILE *fp = fopen(fileName, "rb");
	if (fp)
	{
		printf("get %s 200\n", fileName);
		addString(&str, "HTTP/1.1 200 OK\r\n\r\n");
		int x;
		while ((x = fgetc(fp)) != EOF)
		{
			addChar(&str, (char) x);
		}
		fclose(fp);
	}
	else
	{
		printf("get %s 404\n", fileName);
		addString(&str, "HTTP/1.1 404 Not found\r\n\r\n");
		fp = fopen("404.html", "rb");
		int x;
		while ((x = fgetc(fp)) != EOF)
		{
			addChar(&str, (char) x);
		}
		fclose(fp);
	}
	write(client, str.data, str.ptr);
}

void *handle(void *arg)
{
	int client = *(int *)arg;
	String str = initString();
	char buffer[1024];
	read(client, buffer, sizeof(buffer));
	addString(&str, buffer);
	StringArray array = split(str, "\r\n");
	for (long i = 0; i < getArraySize(array); i++)
	{
		if (strstr(getString(array, i).data, "GET"))
		{
			String url = getString(split(getString(array, i), " "), 1);
			StringArray arr = split(url, "/");
			String fileName = getString(arr, getArraySize(arr) - 1);
			sender(client, fileName.data);
			freeString(url);
			freeString(fileName);
			freeArray(arr);
		}
	}
	freeArray(array);
	freeString(str);
	close(client);
	return NULL;
}

int main()
{
	signal(SIGINT, interrupt);
	server = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind(server, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	listen(server, 1000);
	struct sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);
	while (1)
	{
		int client = accept(server, (struct sockaddr *) &clientAddr, &len);
		printf("connected by %s\n", inet_ntoa(clientAddr.sin_addr));
		pthread_t th;
		pthread_create(&th, NULL, handle, &client);
	}
	close(server);
	return 0;
}
