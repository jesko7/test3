#include "z_chat_client_controller.h"

pthread_mutex_t show_upline_mutex_t;
pthread_cond_t show_upline_cond_t;
pthread_mutex_t exit_mutex_t;
int pthread_is_exit = -1;  //没有退出-1  退出1
int show_upline_condition = 0;
char name[1024] = "\0";

//选择文件？
int is_choose_recv_file = -1;   //没有选择接收文件-1   选择接收文件1
pthread_mutex_t cmd_mutex_t;

extern int up_line_user_count;

char is_admin[32] = {0};

void z_chat_client_controller(const int port, const char *ip)
{
    int sockfd = z_chat_connect_server(port, ip);
    if (sockfd == -1)
        return;

    while (1)
    {
        printf("***********_________________*************\n");
        printf("*****************************************\n");
        printf("****1.Login    1.Login   1.Login*********\n");
        printf("****2.Register 2.Register 2.Register*****\n");
        printf("****3.Quit     3.Quit     3.Quit*********\n");
        printf("*****************************************\n");
        printf("***********_________________*************\n");


        int cmd_first;
        printf("please input your cmd:");
        scanf("%d", &cmd_first);

        if(cmd_first != 3 && cmd_first != 1 && cmd_first != 2)
        {
            printf("input error!!! please input again!\n");
            sleep(2);
            system("clear");
            continue;
        }

        if (cmd_first == 3)
            return;

        memset(name, 0, 1024);
        char password[1024] = "\0";
        printf("plese input user name:\n");
        scanf("%s", name);

        printf("plese input user password:\n");
        getchar();
        InputCode(password);
        //scanf("%s",password);

        if (name == NULL || password == NULL)
        {
            printf("input error\n");
            return;
        }
        printf("Input success name:%s password:%s\n", name, password);

        //if(strncmp(name,"admin",4)==0)

        if (cmd_first == 1)
        {
            if (-1 == register_login_client(sockfd, name, password, -1))
            {
                printf("Login service error\n");
                continue;
            }
            printf("Login succeess\n");
            break;
        }

        else if (cmd_first == 2)
        {
            if (-1 == register_login_client(sockfd, name, password, 1))
            {
                printf("register service error\n");
                continue;
            }
            printf("register succeess plese login\n");
        }


    }

    pthread_mutex_init(&show_upline_mutex_t, NULL);
    pthread_mutex_init(&exit_mutex_t, NULL);
    pthread_mutex_init(&cmd_mutex_t, NULL);
    pthread_cond_init(&show_upline_cond_t, NULL);

    //启动线程专门接收服务器消息
    start_detach_thread(NULL, z_chat_server_msg_handle_thread, &sockfd);
    printf("Wait for Upline Users...\n");
    pthread_mutex_lock(&show_upline_mutex_t);
    while (show_upline_condition == 0)
    {
        pthread_cond_wait(&show_upline_cond_t, &show_upline_mutex_t);
    }
    pthread_mutex_unlock(&show_upline_mutex_t);

    while (1)
    {
        printf("*********************************************************\n");
        printf("**1.send msg to user             2.send msg to all user**\n");
        printf("**3.send file to user            4 quite*****************\n");
        printf("*********************************************************\n");
        printf("**5.(only admin)kill              ***********************\n");
        printf("is_admin = %s\n",is_admin);
        int cmd;
        scanf("%d", &cmd);

        pthread_mutex_lock(&exit_mutex_t);
        if (1 == pthread_is_exit)
        {
            printf("recv service exit programer exit\n");
            pthread_mutex_unlock(&exit_mutex_t);
            break;
        }
        pthread_mutex_unlock(&exit_mutex_t);

        pthread_mutex_lock(&cmd_mutex_t);
        if (is_choose_recv_file == -1)
        {
            if (cmd == 4)
            {
                pthread_mutex_unlock(&cmd_mutex_t);
                break;
            }

            if (cmd == 1)
            {
                z_chat_client_send_msg(sockfd, -1);       //-1私聊
            }

            else if (cmd == 2)
            {
                z_chat_client_send_msg(sockfd, 1);       //1群聊
            }

            else if (cmd == 3)
            {
                z_chat_send_file(sockfd);
            }

            else if (cmd == 5)
            {
                if(strcmp(is_admin,"Y")==0)
                {
                    kill(sockfd);
                }
                else
                {
                    printf("you are not admin,no access!\n");
                }
            }

            else
            {
                printf("input error!! please input again!\n");
                sleep(2);
                system("clear");
                pthread_mutex_unlock(&cmd_mutex_t);
                continue;
            }
        }

        else
        {
            if (cmd == 1)
            {
                z_chat_confirm_recv_file(sockfd);
            }
            else
            {
                z_chat_refuse_recv_file(sockfd);
            }
            is_choose_recv_file = -1;
        }

        pthread_mutex_unlock(&cmd_mutex_t);
    }
}

void z_chat_client_send_msg(int sockfd, int is_send_all)
{
    if (up_line_user_count == 1)
    {
        printf("now has not upline users\n");
        return;
    }

    char msg[1024] = "\0";
    char user[1024] = "\0";
    if (is_send_all == -1)
    {
        printf("plese input client name to send\n");
        scanf("%s", user);
    }

    // if(verify_input_send(user) != 1)
    // {
    //     printf("no find user\n");
    //     return;
    // }

    printf("plese input msg to send\n");
    scanf("%s", msg);

    //构造消息头
    int len = sizeof(struct msg_define);
    struct msg_header header = get_init_header(MSG_SEND, len);

    //构造消息正文
    struct msg_define msg_struct;
    memset(&msg_struct, 0, sizeof(msg_struct));
    msg_struct.is_send_all = is_send_all;
    strcpy(msg_struct.msg, msg);
    strcpy(msg_struct.user_name, user);

    char s_buffer[sizeof(header) + sizeof(msg_struct)] = {0};
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &msg_struct, sizeof(msg_struct));

    //发送消息
    int s_ret = send(sockfd, s_buffer, sizeof(header) + sizeof(msg_struct), 0);
    if (s_ret == -1)
    {
        perror("send msg error\n");
    }
}

//客户端服务器交互层
void z_chat_server_msg_handle_thread(void *arg)
{
    int sockfd = *(int *)(arg);
    int head_len = sizeof(struct msg_header);
    struct msg_header header;
    int time = 5;
    while (1)
    {
        int r_len = recv(sockfd, &header, head_len, 0);
        if (r_len == 0 || r_len == -1)
        {
            //客户端关闭连接
            printf("receive service msg error coutinue:%d\n", time--);
            if (time <= 0)
                break;

            continue;
        }

        if (strncmp((const char *)header.header, "SQJY", 4) == 0)
        {
            //接收正文
            char content[header.content_len];
            r_len = recv(sockfd, content, header.content_len, 0);
            if (r_len == 0 || r_len == -1)
            {
                //客户端关闭连接
                printf("receive service msg error coutinue:%d\n", time--);
                if (time <= 0)
                {
                    break;
                }
                continue;
            }

            //传递下一层处理
            process_server_msg(sockfd, content, header.control_mask);
        }
        time = 5;
    }

    close(sockfd);
    pthread_exit(NULL);

    printf("connect to server has cut plese input any key to exit\n");
    pthread_mutex_lock(&exit_mutex_t);
    pthread_is_exit = 1;
    pthread_mutex_unlock(&exit_mutex_t);
}

void process_server_msg(int sockfd, const char *content, unsigned short control_mask)
{
    switch (control_mask)
    {
    case UPLINE_USERS:
        upline_user_handle(content);
        //在线用户
        break;
    case MSG_SEND:
        msg_handle(content);
        //收到消息
        break;
    case FILE_SEND_REQUEST:
        z_chat_recv_file_request(sockfd, content);
        break;
    case FILE_SEND_REPLAY:
        z_chat_file_send_replay(sockfd, content);
        break;

    default:
            break;
    }
}




//踢人下线函数
void kill(int sockfd)
{
    char killname[1024] = {0};
    printf("请输入被踢人姓名\n");
    scanf("%s", killname);
    //构造消息头
    struct msg_header header;
    memset(&header, 0, sizeof(header));
    strcpy(header.header, "SQJY");
    header.control_mask = KILL;
    int len = sizeof(struct msg_define);
    header.content_len = len;

    //构造禁言正文
    struct msg_define msg_struct;
    memset(&msg_struct, 0, sizeof(msg_struct));
    strcpy(msg_struct.user_name, killname);

    char s_buffer[sizeof(header) + sizeof(msg_struct)] = {0};
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &msg_struct, sizeof(msg_struct));

    //发送消息
    int s_ret = send(sockfd, s_buffer, sizeof(header) + sizeof(msg_struct), 0);
    if (s_ret == -1)
    {
        printf("client kill people error\n");
    }
}