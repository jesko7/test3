#include "z_block_server.h"
#include "z_chat_client_controller.h"

int z_chat_connect_server(const int port, const char *ip)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket error\n");
        return -1;
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        perror("connect server error\n");
        return -1;
    }

    printf("connect server success\n");
    return sockfd;
}