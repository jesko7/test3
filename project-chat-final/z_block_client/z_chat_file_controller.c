#include "z_chat_file_controller.h"

extern char name[1024];
extern int up_line_user_count;
extern int is_choose_recv_file;
extern pthread_mutex_t cmd_mutex_t;

char file_send_user_name[1024] = "\0";
char file_send_path[1024] = "\0";
//发送文件
int z_chat_send_file(int sockfd)
{
    if (up_line_user_count == 1)
    {
        printf("now has not up line users\n");
        return -1;
    }
    char send_to[1024] = "\0";
    printf("plese input user you want send to\n");
    scanf("%s", send_to);

    while (-1 == verify_input_send(send_to))
    {
        printf("input send to user has error plese input again\n");
        scanf("%s", send_to);
    }

    printf("plese input your wann send file path\n");
    char file_path[1024] = "\0";
    scanf("%s", file_path);

    if (file_exist(file_path) == -1)
    {
        printf("the file don't find plese check input\n");
        return -1;
    }

    else
    {
        strcpy(file_send_path,file_path);
        printf("the file path correct\n");
    }

    strcpy(file_send_path, file_path);

    struct file_info_content file_info;
    const int file_content_len = sizeof(struct file_info_content);
    struct msg_header header = get_init_header(FILE_SEND_REQUEST, file_content_len); //发送文件请求

    memset(&file_info,0,sizeof(file_info));
    strcpy(file_info.file_name, get_file_name(file_path));
    strcpy(file_info.recv_user_name, send_to); //发送给谁
    strcpy(file_info.send_user_name, name);    //发的人是自己 A
    file_info.file_size = get_file_size(file_path);
    file_info.file_buffer_length = 0;

    char s_buffer[sizeof(header)+sizeof(file_info)];
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &file_info, sizeof(file_info));

    if (-1 == send(sockfd, s_buffer, sizeof(header)+sizeof(file_info), 0))
    {
        printf("send file request error\n");
        return -1;
    }

    return 0;
}

int z_chat_recv_file_request(int sockfd, const char *content)
{
    struct file_info_content file_info;
    memcpy(&file_info, content, sizeof(file_info)); //拷贝文件信息

    printf("User:%s send your file:%s size:%ld do you wann recv? 1->Yes other->No\n",
           file_info.send_user_name, file_info.file_name, file_info.file_size);

    strcpy(file_send_user_name, file_info.send_user_name); // file_send_user_name A

    pthread_mutex_lock(&cmd_mutex_t);
    is_choose_recv_file = 1;
    pthread_mutex_unlock(&cmd_mutex_t);

    return 1;
}

int z_chat_file_send_replay(int sockfd, const char *content)
{
    struct file_send_replay replay;
    memset(&replay,0,sizeof(replay));
    memcpy(&replay, content, sizeof(replay));

    if (replay.code == CODE_ERROR)
    {
        printf("send file error:%s\n", replay.errmsg);
        return -1;
    }

    else if (replay.code == FILE_RECV_REFUSE)
    {
        //拒绝
        printf("user:%s has refuse you send file\n", replay.recv_user);
        return -1;
    }

    else if (replay.code == FILE_RECV_CONFIRM)
    {
        //确认
        z_chat_confirm_send_file(sockfd, content);
        return 0;
    }
}

int z_chat_refuse_recv_file(int sockfd)
{
    //拒绝 header + replay
    send_file_replay(sockfd, FILE_RECV_REFUSE, NULL);
}

//发送文件回应信息
void send_file_replay(int sockfd, unsigned short replay_msk, const char *err_msg)
{
    struct file_send_replay replay;
    struct msg_header header = get_init_header(FILE_SEND_REPLAY, sizeof(replay));
    replay.code = replay_msk;
    strcpy(replay.send_user, file_send_user_name); // file_send_user_name A
    strcpy(replay.recv_user, name);

    if (err_msg != NULL)
    {
        strcpy(replay.errmsg, err_msg);
    }
    char s_buffer[sizeof(header) + sizeof(replay)] = "\0";
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &replay, sizeof(replay));

    if (-1 == send(sockfd, s_buffer, sizeof(header) + sizeof(replay), 0))
    {
        perror("send refuse recv file error\n");
        return;
    }
    return;
}

//如果出错呢就用oldfd
int z_chat_confirm_recv_file(int oldfd)
{
    start_detach_thread(NULL, recv_file_thread, NULL);
}

//确认发送文件
int z_chat_confirm_send_file(int sockfd, const char *content)
{
    start_detach_thread(NULL, send_file_thread, (void *)content);
}

//发送文件线程 注意发送过快导致TCP缓冲区慢粘包现象
void send_file_thread(void *arg)
{
    printf("stat send file....\n");
    struct file_send_replay replay;

    memcpy(&replay, arg, sizeof(replay));

    //连接客户端
    int file_send_fd = z_chat_connect_server(5555, "192.168.0.136");
    int f_fd = open(file_send_path, O_RDONLY);

    if (-1 == f_fd)
    {
        perror("send file open:");
        close(file_send_fd); //关闭连接
    }

    //一次读文件1024 * 10字节,构造文件发送正文
    struct file_info_content file_content;
    memset(&file_content, 0, sizeof(file_content));
    strcpy(file_content.file_name, get_file_name(file_send_path));
    strcpy(file_content.send_user_name, name);             // A
    strcpy(file_content.recv_user_name, replay.recv_user); // B
    file_content.file_size = get_file_size(file_send_path);
    struct msg_header header = get_init_header(FILE_SEND, sizeof(file_content));

    int read_len_max = 1024 * 10;

    while (1)
    {
        //发送这个文件
        //strncpy(header.header,"SQJY",4);
        char file_buffer[read_len_max];
        memset(file_buffer,0,sizeof(file_buffer));
        int r_len = read(f_fd, file_buffer, read_len_max);
        file_content.file_buffer_length = r_len;

        //发送消息头和消息结构
        char s_buffer[sizeof(header) + sizeof(file_content)];
        memset(s_buffer,0,sizeof(s_buffer));
        memcpy(s_buffer, &header, sizeof(header));
        memcpy(s_buffer + sizeof(header), &file_content, sizeof(file_content));
        if (-1 == send(file_send_fd, s_buffer, sizeof(header) + sizeof(file_content), 0))
        {
            printf("send file head and content error\n");
            break;
        }
        usleep(4000);
        if (0 == r_len)
            break;

        //读到文件
        if (-1 == send(file_send_fd, file_buffer, r_len, 0))
        {
            perror("send file buffer error\n");
            break;
        }
        usleep(4000);
        //printf("%s\n",header.header);
    } 
    close(f_fd);
    close(file_send_fd);
    printf("send file ending....\n");
   
    pthread_exit(NULL);
}

//接收文件线程
void recv_file_thread(void *arg)
{
    //接收文件
    int file_recv_fd = z_chat_connect_server(5555, "192.168.0.136");
    //新连接发送接收文件
    send_file_replay(file_recv_fd, FILE_RECV_CONFIRM, NULL);

    //先接收一次
    struct msg_header header;
    struct file_info_content file_content;
    memset(&file_content, 0, sizeof(file_content));
    char r_buffer[sizeof(header) + sizeof(file_content)] = "\0";
    recv(file_recv_fd, r_buffer, sizeof(header) + sizeof(file_content), 0);
    memcpy(&file_content, r_buffer + sizeof(header), sizeof(file_content));
    char file_buffer[file_content.file_buffer_length];
    memset(file_buffer,0,sizeof(file_buffer));
    recv(file_recv_fd, file_buffer, file_content.file_buffer_length, 0);
    int f_fd = -1;

    //写入文件
    char file_path[1500] = "\0";
    sprintf(file_path, "/home/jsetc/sq_2021/%s", file_content.file_name);
    if (file_exist(file_path))
    {
        memset(file_path, 0, 1500);
        sprintf(file_path, "/home/jsetc/sq_2021/temp_%s", file_content.file_name);
    }
    f_fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, S_IRWXO | S_IRWXU | S_IRWXG);
    if (-1 == f_fd)
    {
        perror("recv file open:");
        close(file_recv_fd);
        return;
    }
    int w_ret = write(f_fd, file_buffer, file_content.file_buffer_length);

    while (1)
    {
        memset(r_buffer, 0, sizeof(header) + sizeof(file_content));
        memset(&file_content, 0, sizeof(file_content));
        int ret = recv(file_recv_fd, r_buffer, sizeof(header) + sizeof(file_content), 0);
        if (-1 == ret)
        {
            perror("recv file error\n");
            break;
        }
        memcpy(&file_content, r_buffer + sizeof(header), sizeof(file_content));
        if (0 == file_content.file_buffer_length)
        {
            break;
        }

        //写入到本地文件
        char file_buffer[file_content.file_buffer_length];
        memset(file_buffer,0,sizeof(file_buffer));
        int r_ret = recv(file_recv_fd, file_buffer, file_content.file_buffer_length, 0);
        if (-1 == r_ret)
        {
            perror("recv file buffer\n");
            break;
        }
        int r_w = write(f_fd, file_buffer, file_content.file_buffer_length);
    }


    close(f_fd);
    close(file_recv_fd);
    printf("recv file ending....\n");
    pthread_exit(NULL);
}