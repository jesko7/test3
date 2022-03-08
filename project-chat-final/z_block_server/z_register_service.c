#include "z_register_service.h"

extern Link client_head;

//注册
void register_handle(int sockfd, const char *content)
{
    if (content == NULL)
        return;
    struct user_struct user_info; //注册正文结构体
    memcpy(&user_info, content, sizeof(struct user_struct));

    struct server_replay replay;
    memset(&replay, 0, sizeof(replay));
    struct msg_header header = get_init_header(REGISTER_REPLAY, sizeof(replay));

    char current[1024] = "\0";
    struct timespec time;
    clock_gettime(0, &time);
    struct tm nowTime;
    localtime_r(&time.tv_sec, &nowTime);
    sprintf(current, "%04d/%02d/%02d", nowTime.tm_year + 1900, nowTime.tm_mon, nowTime.tm_mday);

    int is_exist = user_is_exit_db(user_info);
    if (-1 != is_exist)
    {
        //用户已经存在
        replay.code = CODE_ERROR;
        strcpy(replay.errmsg, "user is exist plese change your name");
    }
    else
    {
        //插入数据库
        char final_sql[1024] = "\0";
        replay.code = SUCCESS;
        char *sql = "insert into users (USER_NAME,USER_PASS,REGISTER_DATA,IS_ADMIN) values ('%s','%s','%s','N')";
        sprintf(final_sql, sql, user_info.user_name, user_info.pass_word, current);

        zchat_exec_sql(final_sql, NULL, NULL, NULL);
    }

    char s_buffer[sizeof(header) + sizeof(replay)] = "\0";
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &replay, sizeof(replay));

    if (-1 == send(sockfd, s_buffer, sizeof(header) + sizeof(replay), 0))
    {
        perror("send login replay to user error\n");
    }
}

//登录
void user_login_handle(int sockfd, const char *content)
{
    if (content == NULL)
        return;
    struct user_struct user_info;
    memcpy(&user_info, content, sizeof(struct user_struct));
    struct server_replay replay;
    struct msg_header header = get_init_header(LOGIN_REPLAY, sizeof(replay));
    memset(&replay, 0, sizeof(replay));

    //用户是否在线
    Link p = client_head;
    int is_upline = -1;
    while (p != NULL)
    {
        if (strcmp(p->user_name, user_info.user_name) == 0)
        {
            replay.code = CODE_ERROR;
            strcpy(replay.errmsg, "your account has already up line!!\n");
            is_upline = 1;
            break;
        }
        p = p->next;
    }

    //用户存在
    int is_exist = -1;
    if (is_upline == -1)
    {
        is_exist = user_is_exit_db(user_info);

        if (-1 == is_exist)
        {
            //用户登录失败
            replay.code = CODE_ERROR;
            strcpy(replay.errmsg, "plese check your account!");
        }

        else
        {
            int ret = user_is_admin(user_info, USER_LOGIN);
            if (ret == 1)
            {
                // admin用户登录成功
                printf("admin:%s account is exist and login success\n", user_info.user_name);
                replay.code = SUCCESS;
                strcpy(replay.errmsg, "Y");
            }
            else
            {
                //普通用户登录成功
                printf("user:%s account is exist and login success\n", user_info.user_name);
                replay.code = SUCCESS;
                strcpy(replay.errmsg, "user");
            }
        }
    }

    //构造消息
    char s_buffer[sizeof(header) + sizeof(replay)] = "\0";
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &replay, sizeof(replay));

    if (-1 == send(sockfd, s_buffer, sizeof(header) + sizeof(replay), 0))
    {
        perror("send login replay to user error\n");
    }

    if (is_exist != -1 && is_upline == -1) //登录成功
    {
        Link new_node;
        create_node(&new_node);
        new_node->sockfd = sockfd;
        memcpy(new_node->user_name, user_info.user_name, 32);
        memcpy(new_node->pass_word, user_info.pass_word, 32);

        //插入
        insert_list_head(&client_head, new_node);

        //返回客户端目前在线的用户
        send_upline_users(-1, 1);
    }
}

int user_is_exit_db(struct user_struct user)
{
    char **query_res;
    char final_sql[1024] = "\0";
    char *sql = "select * from users where USER_NAME = '%s' and USER_PASS = '%s';";
    sprintf(final_sql, sql, user.user_name, user.pass_word, user.is_admin);

    printf("user login query sql:%s\n", final_sql);

    int col;
    int row;
    zchat_query_table(final_sql, query_res, &row, &col, NULL);

    if (row > 0)
    {
        return 1;
    }

    return -1;
}

//用户是否是admin
int user_is_admin(struct user_struct user, unsigned short mask)
{
    char **query_res = NULL;
    char sql[1024];
    if (mask == USER_LOGIN)
    {
        sprintf(sql, "select * from users where USER_NAME ='%s' and IS_ADMIN='Y';", user.user_name);
    }

    int col = 0;
    int row = 0;
    int ret = zchat_query_table(sql, query_res, &row, &col, NULL);
    if (ret != SQLITE_OK)
    {
        printf("select error\n");
    }
    if (row > 0)
    {
        printf("user:%s exist in database suqian.db\n", user.user_name);
        z_chat_free_result(query_res);
        return 1;
    }
    else
    {
        return -1;
    }
}