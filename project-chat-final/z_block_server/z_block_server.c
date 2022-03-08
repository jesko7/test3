#include "z_block_server.h"

int z_socket()
{
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        perror("socket error \n");
        return -1;
    }

    return sockfd;
}

int z_bind(int sockfd, const int port, const char *ip)
{
    if (sockfd <= 0 || port <= 0 || ip == NULL)
        return -1;

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = inet_addr(ip);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //设置端口复用

    int ret = bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr));
    if (ret == -1)
    {
        perror("bind error\n");
        return -1;
    }

    listen(sockfd, 36);

    return sockfd;
}

int z_accept(int sockfd, int *client_port, char client_ip[])
{
    struct sockaddr_in client_sockaddr;
    socklen_t length = sizeof(client_sockaddr);
    bzero(&client_sockaddr, length);

    int cfd = accept(sockfd, (struct sockaddr *)&client_sockaddr, &length);

    if (cfd == -1)
    {
        perror("accept error\n");
        return -1;
    }

    *client_port = ntohs(client_sockaddr.sin_port);
    strcpy(client_ip, inet_ntoa(client_sockaddr.sin_addr));

    return cfd;
}