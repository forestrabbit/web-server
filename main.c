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

String numToStr(long x)
{
	String ans = initString();
	int ptr = 0;
	int data[1000];
	while (x)
	{
		data[ptr++] = x % 10;
		x /= 10;
	}
	for(ptr--; ptr >= 0; ptr--)
	{
		addChar(&ans, (char) (data[ptr] + 0x30));
	}
	return ans;
}

void sender(int client, char *fileName)
{
	String str = initString();
	FILE *fp = fopen(fileName, "rb");
	if (fp)
	{
		String s = initString();
		addString(&s, fileName, strlen(fileName));
		if (isEqual(s, "Release") || isEqual(s, "InRelease") || isEqual(s, "Packages") || isEqual(s, "Release.gpg"))
		{
			printf("304\n", fileName);
			char *t = "HTTP/1.0 304 Not Modified\r\n";
			addString(&str, t, strlen(t));
		}
		else
		{
			printf("200\n", fileName);
			char *t = "HTTP/1.0 200 OK\r\n";
			addString(&str, t, strlen(t));
		}
		freeString(s);
		addString(&str, "Content-Length: ", 16);
		int x;
		long len = 0;
		String temp = initString();
		while ((x = fgetc(fp)) != EOF)
		{
			len++;
			addChar(&temp, (char) x);
		}
		String l = numToStr(len);
		addString(&str, toCString(l), getStringSize(l));
		addString(&str, "\r\n\r\n", 4);
		addString(&str, toCString(temp), getStringSize(temp));
		freeString(l);
		freeString(temp);
		fclose(fp);
	}
	else
	{
		printf("404\n", fileName);
		addString(&str, "HTTP/1.1 404 Not Found\r\n\r\n", 26);
		fp = fopen("404.html", "rb");
		int x;
		while ((x = fgetc(fp)) != EOF)
		{
			addChar(&str, (char) x);
		}
		fclose(fp);
	}
	write(client, toCString(str), str.ptr);
	freeString(str);
}

void *handle(void *arg)
{
	int client = *(int *)arg;
	String str = initString();
	char buffer[1024];
	read(client, buffer, sizeof(buffer));
	addString(&str, buffer, strlen(buffer));
	StringArray array = split(str, "\r\n");
	for (long i = 0; i < getArraySize(array); i++)
	{
		if (strstr(toCString(getString(array, i)), "GET"))
		{
			String url = getString(split(getString(array, i), " "), 1);
			StringArray arr = split(url, "/");
			printf("get %s ", toCString(url));
			if (getArraySize(arr) == 2)
			{
				String fileName = getString(arr, 1);
				sender(client, toCString(fileName));
				freeString(fileName);
			}
			else if (getArraySize(arr) == 3 && isEqual(getString(arr, 1), "."))
			{
				String fileName = getString(arr, 2);
				sender(client, toCString(fileName));
				freeString(fileName);
			}
			freeString(url);
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
	char *addr = "127.0.0.1";
	int port = 8080;
	signal(SIGINT, interrupt);
	server = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(addr);
	int x = bind(server, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (x != -1)
	{
		printf("Listen at %s:%d\n", addr, port);
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
	}
	else
	{
		printf("%s:%d cannot be used\n", addr, port);
	}
	close(server);
	return 0;
}
