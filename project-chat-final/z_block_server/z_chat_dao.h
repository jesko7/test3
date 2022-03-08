#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

sqlite3 *p_db;

int z_chat_open_db(const char *db_dile_path);

int zchat_exec_sql(const char *sql, sqlite3_callback callback, void *callback_arg, char **errmsg);

int zchat_query_table(const char *sql, char **result, int *row, int *col, char **errmsg);

void z_chat_free_result(char **result);

void zchat_close_db();