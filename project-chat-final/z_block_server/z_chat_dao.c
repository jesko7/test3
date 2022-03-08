#include "z_chat_dao.h"

//打开数据库
int z_chat_open_db(const char *db_file_path)
{
    int ret = sqlite3_open(db_file_path, &p_db);
    if (ret != SQLITE_OK)
    {
        printf("open sqlite:%s error\n", db_file_path);
        return -1;
    }
    return ret;
}

//使用回调函数执行sql语句
int zchat_exec_sql(const char *sql, sqlite3_callback callback, void *callback_arg, char **errmsg)
{
    char *msg;
    int ret = sqlite3_exec(p_db, sql, callback, callback_arg, &msg);
    return ret;
}

//不使用回调函数执行sql语句
int zchat_query_table(const char *sql, char **result, int *row, int *col, char **errmsg)
{
    char *msg;
    int ret = sqlite3_get_table(p_db, sql, &result, row, col, &msg);
    return ret;
}

void z_chat_free_result(char **result)
{
    sqlite3_free_table(result);
}

void zchat_close_db()
{
    sqlite3_close(p_db);
}