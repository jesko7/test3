#include "z_chat_server_controller.h"

//全局客户端链表
Link client_head = NULL;
Sending_File_Link sending_file_head = NULL;

int z_start_server(const int port, const char *ip)
{
    //打开数据库，不存在则创建
    int ret = z_chat_open_db("/home/jsetc/sq_2021/project-chat-final/suqian_chat.db");
    if (ret != SQLITE_OK)
    {
        printf("open sqlite3 error");
        return -1;
    }
    printf("open sqlite3 success\n");

    // TCP连接，创建套接字，绑定
    int sockfd = z_socket();
    if (sockfd == -1)
    {
        return -1;
    }

    if ((sockfd = z_bind(sockfd, port, ip)) == -1)
    {
        return -1;
    }
    printf("sockfd = %d\n", sockfd);

    printf("waiting for client....\n");

    while (1)
    {
        char c_ip[16] = "\0";
        int c_port;

        int cfd = z_accept(sockfd, &c_port, c_ip); //通过指针传递才能修改原来的值
        if (-1 == cfd)
            continue;

        printf("accept client port:%d ip:%s connect:%d\n", c_port, c_ip, cfd);
        start_detach_thread(NULL, zchat_server_controller, &cfd);
        printf("创建线程成功\n");
        //连接上服务器
    }

    file_list_destory(sending_file_head);
    destory(client_head);
    // close(sockfd);
    // file_list_destory(sending_file_head);
    zchat_close_db();
}

//客户端服务端交互控制层
void *zchat_server_controller(void *arg)
{
    int sockfd = *(int *)(arg);
    int head_len = sizeof(struct msg_header);
    struct msg_header header;
    while (1)
    {

        int r_len = recv(sockfd, &header, head_len, 0);
        if (r_len == 0 || r_len == -1)
        {
            //客户端关闭连接
            printf("client %d cut connection\n", sockfd);
            break;
        }
        printf("head = %s\n", header.header);
        //解析消息头
        if (strncmp((const char *)header.header, "SQJY", 4) == 0) //防止客户端恶意访问服务端，保护服务端不被攻击
        {
            //符合协议，处理消息
            if (verify_header(header) == -1)
                continue;

            //接收正文

            char content[header.content_len]; //准确拿到数据大小，防止TCP粘包
            r_len = recv(sockfd, content, header.content_len, 0);
            if (r_len == 0 || r_len == -1)
            {
                //客户端关闭连接
                printf("client %d cut connection\n", sockfd);
                break;
            }

            //传递下一层处理
            process_client_msg(sockfd, content, header.control_mask);
        }

        else
        {
            printf("head = %s\n", header.header);
            printf("client %d send error\n", sockfd);
            break;
        }
    }

    //用户下线
    delete_node(&client_head, sockfd);

    close(sockfd);

    //给其他用户发送在线用户情况
    send_upline_users(-1, 1);

    pthread_exit(NULL);
}

//校验消息头
int verify_header(struct msg_header header)
{
    if (header.content_len == -1)
        return -1;

    if (header.control_mask == -1)
        return -1;

    return 0;
}

//提供服务
void process_client_msg(int sockfd, const char *content, unsigned short control_mask)
{
    switch (control_mask)
    {
    case USER_LOGIN: //登录
        user_login_handle(sockfd, content);
        break;

    case REGISTER: //注册
        register_handle(sockfd, content);
        break;

    case MSG_SEND: //发送消息
        send_msg_handle(sockfd, content);
        break;

    case FILE_SEND_REQUEST: //文件发送请求
        file_send_request_service(sockfd, content);
        break;

    case FILE_SEND_REPLAY: //文件发送回应
        file_send_replay(sockfd, content);
        break;
    case FILE_SEND: //文件发送
        file_send_service(sockfd, content);
        break;

    case KILL:
        kill_recv(sockfd,content);

    default:
        break;
    }
}