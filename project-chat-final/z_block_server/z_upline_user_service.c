#include "z_upline_user_service.h"

extern Link client_head;

int send_upline_users(int sockfd, int is_sendall)   //is_sendll = 1 发送所有的在线用户
{
    //返回客户端目前在线的用户
    struct online_user users;
    memset(&users, 0, sizeof(users));
    users.count = 0;
    int count = 0;
    Link p = client_head;
    int fds[1024];
    if (is_sendall == -1)
    {
        fds[0] = sockfd;
    }
    while (p != NULL)
    {
        memcpy(users.user_names[count], p->user_name, 32);
        printf("upline client ip:%s port:%s fd:%d\n", p->user_name, p->pass_word, p->sockfd);
        if (is_sendall != -1)
        {
            fds[count] = p->sockfd;
        }

        count++;
        p = p->next;
    }

    users.count = count;

    struct msg_header header;
    header.control_mask = UPLINE_USERS;
    header.content_len = sizeof(users);
    memcpy(header.header, "SQJY", 4);
    char s_buffer[sizeof(users) + sizeof(header)];
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &users, sizeof(users));

    int for_count = (is_sendall == -1) ? 1 : count;
    for (size_t i = 0; i < for_count; i++)
    {
        printf("send upline users to client %d\n", fds[i]);
        int s_ret = send(fds[i], s_buffer, sizeof(users) + sizeof(header), 0);
        if (s_ret == -1 || s_ret == 0)
        {
            printf("send online users error\n");
            return -1;
        }
    }

    return 1;
}