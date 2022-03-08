#include "z_msg_send_service.h"

extern Link client_head;

void send_msg_handle(int sockfd, const char *content)
{
    struct msg_define msg_struct;
    memset(&msg_struct, 0, sizeof(msg_struct));
    memcpy(&msg_struct, content, sizeof(msg_struct));

    printf("client%d send %s msg:%s is_send_all:%d\n", sockfd, msg_struct.user_name, msg_struct.msg, msg_struct.is_send_all);

    //找到用户发送的sockfd
    int fds[1024];
    char send_user[1024] = "\0";
    Link p = client_head;
    int count = 0;
    int for_count = 0;
    while (p != NULL)
    {

        printf("upline client ip:%s port:%s fd:%d\n", p->user_name, p->pass_word, p->sockfd);

        if (p->sockfd == sockfd) //找到需要发送的用户
        {
            strcpy(send_user, p->user_name);
        }

        // if(p->silence_flag == 0)
        // {

        // }

        if (msg_struct.is_send_all == 1)   //群发
        {
            fds[count] = p->sockfd;
        }

        else   //私聊
        {
            //找到目标用户
            if (strcmp(p->user_name, msg_struct.user_name) == 0)
            {
                printf("compare send user:%s\n", p->user_name);
                fds[0] = p->sockfd;
                for_count = 1;
            }

            else
            {
                printf("no find user\n");
            }
        }
        count++;
        p = p->next;
    }

    for_count = (msg_struct.is_send_all == 1) ? count : for_count;
    for (size_t i = 0; i < for_count; i++)
    {
        int fd = fds[i];

        struct msg_define s_msg_struct;
        memset(&s_msg_struct, 0, sizeof(s_msg_struct));
        struct msg_header header = get_init_header(MSG_SEND, sizeof(s_msg_struct));

        strcpy(s_msg_struct.msg, msg_struct.msg);
        strcpy(s_msg_struct.user_name, send_user);
        s_msg_struct.is_send_all = msg_struct.is_send_all;

        char s_buffer[sizeof(s_msg_struct) + sizeof(header)] = "\0";
        memcpy(s_buffer, &header, sizeof(header));
        memcpy(s_buffer + sizeof(header), &s_msg_struct, sizeof(s_msg_struct));

        //发送消息
        int s_ret = send(fd, s_buffer, sizeof(s_msg_struct) + sizeof(header), 0);
        if (s_ret == -1)
        {
            perror("send client msg error\n");
        }
    }
}




//踢人函数
void kill_recv(int sockfd,const char *content)
{
    struct msg_define msg_struct;
    memset(&msg_struct,0,sizeof(msg_struct));
    memcpy(&msg_struct,content,sizeof(msg_struct));

    printf("client%d kill %s \n",sockfd,msg_struct.user_name);

    //找到用户发送的sockfd
    int fd=0;//存储所有客户端的套接口
    char send_user[50]={0};
    Link p=client_head;
    while(p!=NULL)
    {   
        if(p->sockfd==sockfd)
        {
            strcpy(send_user,p->user_name);
        }       
        if(strcmp(p->user_name,msg_struct.user_name)==0)
        {
            printf("compare kick user %s\n",p->user_name);
            fd=p->sockfd;
        }       
        p=p->next;
    }
    printf("被踢人的fd:%d\n",fd);
    delete_node(&client_head,fd);
    close(fd);
    //给其他用户发送在线用户情况
    send_upline_users(-1,1);//有用户下线，群发给所有在线用户
}