#ifndef _AUTH_H_
#define _AUTH_H_

#include <stdbool.h>

bool login(const char *username, const char *password, const char *cmd, pid_t *child_pid);
bool logout(void);

#endif
