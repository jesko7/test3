#include "register_controller.h"

extern char is_admin[32];

int register_login_client(int sockfd, const char *name, const char *password, int is_reg)
{
    unsigned short mask = (is_reg == 1) ? REGISTER : USER_LOGIN;

    int len = sizeof(struct user_struct);
    struct msg_header header = get_init_header(mask, len);

    struct user_struct user;
    memset(&user, 0, len);
    strcpy(user.user_name, name);
    strcpy(user.pass_word, password);    
    if(strncmp(user.user_name,"admin",5) == 0)
    {
        strcpy(user.is_admin,"Y");
        strcpy(is_admin,"Y");
        sleep(1);
        system("clear");
        printf("!!!!!welcome my administer!!!!!\n");
        printf("!!!!!you can do any thing!!!!!!\n");
    }
    else
    {

        strcpy(user.is_admin,"N");
        strcpy(is_admin,"N");
        sleep(1);
        system("clear");
        printf("hello ordinary user\n");
    }
    



    char s_buffer[len + sizeof(header)];
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &user, len);

    if (-1 == send(sockfd, s_buffer, len + sizeof(header), 0))
    {
        perror("send register_login msg error\n");
        return -1;
    }

    struct server_replay replay;
    memset(&header, 0, sizeof(header));
    memset(&replay, 0, sizeof(replay));
    char r_buffer[sizeof(header) + sizeof(replay)] = "\0";

    if (-1 == recv(sockfd, r_buffer, sizeof(header) + sizeof(replay), 0))
    {
        perror("recv register_login msg error\n");
        return -1;
    }

    //解析消息
    memcpy(&header, r_buffer, sizeof(header));

    if (strncmp((const char *)header.header, "SQJY", 4) != 0)
    {
        printf("register_login alays header.header error\n");
        return -1;
    }

    memcpy(&replay, r_buffer + sizeof(header), sizeof(replay));
    if (replay.code == CODE_SUCCESS)
    {
        strcpy(is_admin,replay.error_msg);
        return 1;
    }
        

    if (replay.code == CODE_ERROR)
    {
        printf("register_login error:%s\n", replay.error_msg);
        return -1;
    }
}