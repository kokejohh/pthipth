#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "pthipth.h"

#define SERV_IP		"127.0.0.1"
#define SERV_PORT 	2091

#define	MAXLINE	1000

int accept_cr(int fd, struct sockaddr *addr, socklen_t *len);
int write_full(int fd, const void *buf, size_t count);
int read_full(int fd, void *buf, size_t count);

int client_shutdown_flag = 0;

int conn_fd;
struct sockaddr_in serv_addr;

int main(int argc, char *argv[]){
        int n, m;

	fd_set base_rfds, rfds;
        int fdmax = 0;

        char line[MAXLINE];

	conn_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);

        if (connect(conn_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0) {
            perror("Problem in connecting to the server");
            exit(3);
        }

	pthipth_t id;
	read_full(conn_fd, &id, sizeof(id));
	printf("receiv id %d\n", id);

	FD_ZERO(&base_rfds);
	FD_ZERO(&rfds);

	FD_SET(fileno(stdin), &base_rfds);
	FD_SET(conn_fd, &base_rfds);

	fdmax = conn_fd;

	while(1){
	  memcpy(&rfds, &base_rfds, sizeof(fd_set));
	  printf("tid-%03d:> ", id);
          fflush(stdout);

         if (select(fdmax+1, &rfds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
          }

	  if(FD_ISSET(fileno(stdin), &rfds)){
            if(fgets(line, MAXLINE, stdin) == NULL){
	      printf("Shutdown writing to TCP connection\n");
	      shutdown(conn_fd, SHUT_WR);
	      client_shutdown_flag = 1;
	    } else{
              n = write_full(conn_fd, line, MAXLINE);
	    }
	  }

	  if(FD_ISSET(conn_fd, &rfds)){
            if(read_full(conn_fd, line, MAXLINE) == 0){
	      if(client_shutdown_flag){
	        printf("TCP connection closed after client shutdown\n");
	        close(conn_fd);
	        break;
	      } else{
	        printf("Error: TCP connection closed unexpectedly\n");
	        exit(1);
	      }
	    } else{
              fputs(line, stdout);
	    }
	  }
	}
}

// -----Basic Communication Utilities-----
int accept_cr(int fd, struct sockaddr *addr, socklen_t *len){
	int ret;
repeat_accept:
        ret = accept(fd, addr, len); //waiting for connection
        if (ret < 0) { //signal handler
            if (errno == EINTR){
                goto repeat_accept;
	    }
	    printf("accept error errno=%d fd=%d\n", errno, fd); //show error message
        }
	return ret;
}

int write_full(int fd, const void *buf, size_t count){
    ssize_t ret = 0;
    ssize_t total = 0;

    while (count) {
        ret = write(fd, buf, count); //waiting for connection
        if (ret < 0) { //signal handler
            if (errno == EINTR){
                continue; //start new loop
	    }
	    printf("write error errno=%d fd=%d\n", errno, fd); //show error message
            return ret;
        }
        else if (ret == 0){
            return ret;
        }

        count -= ret;
        buf += ret;
        total += ret;
    }

    return total;
}

int read_full(int fd, void *buf, size_t count){
    ssize_t ret = 0;
    ssize_t total = 0;

    while (count) {
        ret = read(fd, buf, count); //waiting for connection
        if (ret < 0) { //signal handler
            if (errno == EINTR){
                continue; //start new loop
	    }
	    printf("read error errno=%d fd=%d\n", errno, fd); //show error message
            return ret;
        }
        else if (ret == 0){
            return ret;
        }

        count -= ret;
        buf += ret;
        total += ret;
    }

    return total;
}
