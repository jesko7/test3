#include "common_utils.h"

extern int up_line_user_count;
extern char name[1024];
extern int upline_users[1023][32];

int start_detach_thread(pthread_t *thread, void *start_functio, void *arg)
{
    pthread_attr_t attr;
    pthread_t thread_t;
    int ret = 0;

    if (thread == NULL)
    {
        thread = &thread_t;
    }

    //初始化线程
    pthread_attr_init(&attr);

    //设置线程的detach属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    ret = pthread_create(thread, &attr, (void *(*)(void *))start_functio, arg);
    if (ret < 0)
    {
        printf("pthread create eeeor:\n");
        goto err;
    }
    pthread_detach(thread_t);

err:
    pthread_attr_destroy(&attr);
    return 1;
}

struct msg_header get_init_header(unsigned short msk, int len)
{
    struct msg_header header;
    memset(&header, 0, sizeof(header));
    strncpy(header.header, "SQJY",4);
    header.control_mask = msk;
    header.content_len = len;
    return header;
}

int file_exist(const char *file_path)
{
    if (file_path == NULL)
        return -1;

    if (access(file_path, F_OK) != -1) //判断文件是否存在
    {
        return 1;
    }
    return -1;
}

unsigned long get_file_size(const char *path)
{
    unsigned long file_size = -1;

    struct stat buffer;

    if (stat(path, &buffer) < 0)
    {
        return file_size;
    }

    return buffer.st_size;
}



char *get_file_name(const char *path)
{
    return basename(path);
}

int verify_input_send(const char *input_user)
{
    if (up_line_user_count <= 1)   //如果只有一个人在线，-1
        return -1;

    if (strcmp(input_user, name) == 0)   //输入自己，-1
        return -1;

    for (size_t i = 0; i < up_line_user_count; i++)
    {
        if (strcmp(upline_users[i], input_user) == 0)
            return 1;
    }

    //return -1;
}


void InputCode(char *password)
{
	int i=0;
	system("stty -icanon");                   //设置一次性读完操作，即getchar()不用回车也能获取字符
	system("stty -echo");                     //关闭回显，即输入任何字符都不显示
	while(i < 16)
	{
		password[i]=getchar();                    //获取键盘的值到数组中
		if(i == 0 && password[i] == 127)
		{
			i=0;                              //若开始没有值，输入删除，则，不算值
			password[i]='\0';
			continue;
		}
		else if(password[i] == 127)
		{
			printf("\b \b");                    //若删除，则光标前移，输空格覆盖，再光标前移
			password[i]='\0';
			i=i-1;                              //返回到前一个值继续输入
			continue;                           //结束当前循环
		}
		else if(password[i] == '\n')               //若按回车则，输入结束
		{
			password[i]='\0';
			break;
		}
		else
		{
			printf("*");
		}
		i++;
	}
	system("stty echo");                    //开启回显
	system("stty icanon");                   //关闭一次性读完操作，即getchar()必须回车也能获取字符
    putchar(10);
	return ;                            //返回最后结果
}

