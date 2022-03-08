#include "z_file_send_service.h"

int i=0;
extern Link client_head;
extern Sending_File_Link sending_file_head;
//文件发送请求
int file_send_request_service(int sockfd, const char *content)
{
    //得到文件发送结构体
    struct file_info_content file_info;
    const int len = sizeof(file_info);
    memset(&file_info, 0, len);
    memcpy(&file_info, content, len);

    int s_fd = -1;
    Link p = client_head;
    char send_user[1024] = "\0";

    while (p != NULL)
    {
        //找到目标用户 
        if (strcmp(p->user_name, file_info.recv_user_name) == 0)
        {
            printf("compare recv user:%s\n", p->user_name);
            s_fd = p->sockfd;
        }
        p = p->next;
    }

    struct server_replay replay;
    struct msg_header header = get_init_header(FILE_SEND_REPLAY, sizeof(replay));

    if (s_fd == -1)
    {
        replay.code = CODE_ERROR;
        strcpy(replay.errmsg, "no find you wann users!");
        printf("no find send users:%s to send", file_info.recv_user_name);
        //没找到用户
        char s_buffer[sizeof(header) + sizeof(replay)] = "\0";
        memcpy(s_buffer, &header, sizeof(header));
        memcpy(s_buffer + sizeof(header), &replay, sizeof(replay));

        if (-1 == send(sockfd, s_buffer, sizeof(header) + sizeof(replay), 0))
        {
            perror("send send file replay msg error\n");
            return -1;
        }
        return 0;
    }

    header = get_init_header(FILE_SEND_REQUEST, len);
    char s_buffer[sizeof(header) + sizeof(file_info)] = "\0";
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &file_info, len);
    if (-1 == send(s_fd, s_buffer, sizeof(header) + sizeof(file_info), 0)) //发给要接收的fd
    {
        perror("send send file request error\n");
        return -1;
    }

    return 0;
}


int file_send_replay(int sockfd, const char *content)
{
    struct file_send_replay replay;
    memset(&replay, 0, sizeof(replay));
    memcpy(&replay, content, sizeof(replay));
    int s_fd = -1;
    Link p = client_head;
    while (p != NULL)
    {
        //找到目标用户
        if (strcmp(p->user_name, replay.send_user) == 0)
        {
            printf("compared send user:%s\n", p->user_name);
            s_fd = p->sockfd;
            break;
        }

        p = p->next;
    }

    struct msg_header header = get_init_header(FILE_SEND_REPLAY, sizeof(replay));
    char s_buffer[sizeof(header) + sizeof(replay)] = "\0";
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &replay, sizeof(replay));

    if (-1 == send(s_fd, s_buffer, sizeof(header) + sizeof(replay), 0))
    {
        perror("send send file request error\n");
        return -1;
    }

    //判断是否接收
    if (replay.code == FILE_RECV_CONFIRM)
    {
        //插入到在线发送文件链表中
        Sending_File_Link new_node;
        f_create_node(&new_node);
        new_node->recv_fd = sockfd;
        memcpy(new_node->send_user_name, replay.send_user, 32);
        memcpy(new_node->recv_user_name, replay.recv_user, 32);
        //插入
        f_insert_list_head(&sending_file_head, new_node);
        printf("%s -> %s,插入正在发送文件链表!\n", replay.send_user, replay.recv_user);
    }
    return 0;
}


int file_send_service(int sockfd, const char *content)
{
    struct file_info_content file_content;
    const int len = sizeof(file_content);
    memset(&file_content, 0, len);
    memcpy(&file_content, content, len);
    struct msg_header header = get_init_header(FILE_SEND, sizeof(file_content));

    //先找到文件接收的fd
    int s_fd = -1;
    Sending_File_Link p = sending_file_head;
    while (p != NULL)
    {
        if (strcmp(p->send_user_name, file_content.send_user_name) == 0 &&
            strcmp(p->recv_user_name, file_content.recv_user_name) == 0)
        {
            i++;
            printf("compared file recv user %s,i=%d\n", p->recv_user_name,i);
            s_fd = p->recv_fd;
            break;
        }
        p = p->next;
        usleep(10);
    }

    if (-1 == s_fd)
    {
        printf("can not find send file user fd!!!\n");
        return -1;
    }

    char s_buffer[sizeof(header) + sizeof(file_content)];  //正文和头的大小
    memset(s_buffer,0,sizeof(s_buffer));
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &file_content, sizeof(file_content));
    if (-1 == send(s_fd, s_buffer, sizeof(header) + sizeof(file_content), 0))
    {
        perror("send file head and content error\n");
        return -1;
    }

    int buffer_len = file_content.file_buffer_length;   //支持的长度，文件的长度
    if (0 != buffer_len)    //接收正文
    {
        char file_buffer[file_content.file_buffer_length];
        memset(file_buffer,0,sizeof(file_buffer));
        if (-1 == recv(sockfd, file_buffer, file_content.file_buffer_length, 0))  //接收A的文件内容
        {
            perror("recv file buffer error\n");
        }
        usleep(4000);
        if (-1 == send(s_fd, file_buffer, buffer_len, 0))    //发送给B端
        {
            perror("send file buffer error\n");
            return -1;
        }
    }

    else
    {
        i = 0;
        printf("file send ending....\n");
        f_delete_node(&sending_file_head,file_content.recv_user_name,file_content.send_user_name);
    }
}