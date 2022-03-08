
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>

int z_socket(); //创建套接字

int z_bind(int sockfd, const int port, const char *ip); //绑定套接字

int z_accept(int sockfd, int *client_port, char client_ip[]); //接收客户端连接