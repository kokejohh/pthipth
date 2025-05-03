#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include "pthipth.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 2091
#define MAXLINE 1000
#define MAX_CONNECTIONS 10

int lis_fd;
int conn_fd[MAX_CONNECTIONS] = {0};

pthipth_t tid[MAX_CONNECTIONS];

struct sockaddr_in serv_addr;

void *connect_serv(void *t);

void listener(void *arg)
{
    lis_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(lis_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
	fprintf(stderr, "bind error\n");
	pthipth_exit(NULL);
    }

    listen(lis_fd, 5);
    printf("chat server starting...\n");

    int cid = 0;
    while (1)
    {
	int tmp_fd;

	if (cid == MAX_CONNECTIONS)
	{
	    fprintf(stderr, "max connection\n");
	    break;
	}

	if ((conn_fd[cid] = accept(lis_fd, NULL, NULL)) < 0)
	{
	    fprintf(stderr, "accept error\n");
	    break;
	}

	tmp_fd = conn_fd[cid];

	pthipth_attr_t attr = {0};
	attr.time_quota_ms = 1;

	if (pthipth_create(&tid[cid], &attr, &(pthipth_task_t){connect_serv, (void *)(long)tmp_fd, DEFAULT_PRIORITY}) != 0)
	{
	    fprintf(stderr, "thread error\n");
	    break;
	}
	cid = (cid + 1) % MAX_CONNECTIONS;
    }
    close(lis_fd);
}

int main(int argc, char *argv[])
{
    pthipth_t tid;

    pthipth_attr_t attr = {0};
    attr.time_quota_ms = 1;

    pthipth_task_t task = {(void *)listener, NULL, DEFAULT_PRIORITY};

    pthipth_create(&tid, &attr, &task);

    pthipth_join(tid, NULL);
}

void *connect_serv(void *t)
{
    int n, m;
    char line[MAXLINE];

    int tmp_fd = (int)(long)t;

    pthipth_t tid = pthipth_self();

    printf("tid %d connected, fd %d\n", tid, tmp_fd);

    for (int i = 0; i < MAX_CONNECTIONS; i++)
	if (conn_fd[i] == tmp_fd)
	{
	    m = i;
	    break;
	}

    write(tmp_fd, &tid, sizeof(tid));

    while ((n = read(tmp_fd, line, MAXLINE)) != 0)
    {
	char str[MAXLINE + MAXLINE];

	sprintf(str, "\r\33[32mtid %d-say: %s\33[0m", tid, line);

	fputs(str, stdout);

	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
	    if (conn_fd[i] && conn_fd[i] != tmp_fd)
	    {
		write(conn_fd[i], str, MAXLINE);
	    }
	}
    }

    if (n == 0) 
    {
	printf("tid %d disconnected\n", tid);
	conn_fd[m] = 0;
    }

    close(tmp_fd);
    return NULL;
}
