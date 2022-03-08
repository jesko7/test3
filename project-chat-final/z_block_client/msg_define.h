#pragma once

#define BACKSPACE  0x7f   //删除的asccll码值

#define REGISTER 0x11
#define USER_LOGIN 0x12
#define LOGIN_REPLAY 0x13
#define REGISTER_REPLAY 0x14
#define KILL 0x15

#define UPLINE_USERS 0x22

#define CODE_ERROR 0x44
#define CODE_SUCCESS 0x55

#define SUCCESS 0x99
#define ERROR 0x98

#define MSG_SEND 0x33
#define MSG_RECV 0x66

#define FILE_SEND 0x60
#define FILE_SEND_REQUEST 0x55
#define FILE_SEND_REPLAY 0x57 //发送文件出错

#define FILE_RECV_REFUSE 0x58  //拒绝
#define FILE_RECV_CONFIRM 0x59 //确认

/*协议头+协议正文+（正文内容）
防止随便读取1024*/

struct msg_header
{
    char header[5]; // 消息的起始标志 SQJY

    unsigned short control_mask; //具体请求服务端进行的操作

    unsigned int content_len; //正文的长度

    unsigned short reservert; //保留位
};

//注册正文结构体
struct user_struct
{
    unsigned char user_name[32];

    unsigned char pass_word[32];

    unsigned char is_admin[10];   //1:管理员  0:普通用户
};

struct server_replay
{
    unsigned char error_msg[100];

    unsigned short code;
};

struct online_user
{
    unsigned char user_names[1023][32];
    unsigned short count;
};

struct msg_define
{
    unsigned char user_name[1024];
    unsigned char msg[1024];
    int is_send_all;
};

//文件信息
//消息头 + 文件信息 + 真正的文件流
struct file_info_content
{
    unsigned char recv_user_name[32];

    unsigned char send_user_name[32];

    unsigned char file_name[1024];

    unsigned long file_size; //表示文件大小

    unsigned int file_buffer_length; //表示后续流的大小，并且做出响应
};

struct file_send_replay
{
    unsigned char recv_user[32];

    unsigned char send_user[32];

    unsigned char errmsg[1024];

    unsigned short code;
};