#include "upline_user_controller.h"

extern pthread_mutex_t show_upline_mutex_t;
extern pthread_cond_t show_upline_cond_t;
extern int show_upline_condition;
extern char name[1024];

int up_line_user_count = 0;
unsigned char upline_users[1023][32];

void upline_user_handle(const char *content)
{
    pthread_mutex_lock(&show_upline_mutex_t);
    struct online_user users;
    memset(&users, 0, sizeof(users));
    memcpy(&users, content, sizeof(struct online_user));

    up_line_user_count = users.count;

    if (users.count <= 1)
    {
        printf("Now has not upline users wait or key Q/q to quit programa\n");
        show_upline_condition = -1;
        pthread_cond_signal(&show_upline_cond_t);
        pthread_mutex_unlock(&show_upline_mutex_t);
        return;
    }

    printf("Now upline users:\n");

    for (size_t i = 0; i < users.count; i++)
    {
        const char *s_name = users.user_names[i];
        strcpy(upline_users[i], s_name);
        if (strncmp(s_name, name, 32) == 0)
        {
            continue;
        }
        printf("%s\n", s_name);
    }
    show_upline_condition = 1;
    pthread_cond_signal(&show_upline_cond_t);
    pthread_mutex_unlock(&show_upline_mutex_t);
}