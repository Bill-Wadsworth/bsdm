#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include <sys/types.h>
#include <grp.h>

#include <pwd.h>
#include <paths.h>
#include <string.h>

#include <stdlib.h>

#include "auth.h"

#define SERVICE_NAME "display_manager"

static void init_env(struct passwd *pw);
static void set_env(char *name, char *value);
static int end(int last_result);

static int conv(int num_mesg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr);

static pam_handle_t *pam_handle;

/* Helper function for login to determine and act upon any PAM errors that may happen */
bool pam_err(int result, char* name) {
    if (result != PAM_SUCCESS) {
        fprintf(stderr, "%s: %s\n", name, pam_strerror(pam_handle, result));
        end(result);
        return true;
    }
    return false;
}

bool login(const char *username, const char *password, pid_t *child_pid) {
    const char *data[2] = {username, password};
    struct pam_conv pam_conv = {
        conv, data
    };

    int result = pam_start(SERVICE_NAME, username, &pam_conv, &pam_handle);
    if (pam_err(result, "pam_start")) return false;

    result = pam_authenticate(pam_handle, 0);
    if (pam_err(result, "pam_authenticate")) return false;

    result = pam_acct_mgmt(pam_handle, 0);
    if (pam_err(result, "pam_acct_mgmt")) return false;

    result = pam_setcred(pam_handle, PAM_ESTABLISH_CRED);
    if (pam_err(result, "pam_setcred")) return false;

    result = pam_open_session(pam_handle, 0);
    if (result != PAM_SUCCESS) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
    }
    if (pam_err(result, "pam_open_session")) return false;
    
    
    struct passwd *pw = getpwnam(username);

    *child_pid = fork();
    if (*child_pid == 0) {
        init_env(pw);
        //try setting some privaliges in the child
        setenv("HOME", pw->pw_dir, 1);
        setenv("USER", pw->pw_name, 1);
        setenv("LOGNAME", pw->pw_name, 1);
        setenv("SHELL", pw->pw_shell, 1);

        initgroups(pw->pw_name, pw->pw_gid);
        setgid(pw->pw_gid);
        setuid(pw->pw_uid);

        printf("DONE WITH PERMS\n");

        chdir(pw->pw_dir);
        char *cmd = "exec /bin/bash --login startx";
        execl(pw->pw_shell, pw->pw_shell, "-c", cmd, NULL);
        printf("Failed to run specified commands");
        exit(1);
    }
    return true;
}

static int conv(int num_mesg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
    *resp = calloc(num_mesg, sizeof(struct pam_response));
    if (*resp == NULL) {
        return PAM_BUF_ERR;
    }
    int result = PAM_SUCCESS;
    for (int i=0; i < num_mesg; i++) {
        char *username;
        char *password;
        switch(msg[i]->msg_style) {
            case PAM_PROMPT_ECHO_ON:
                username = ((char **) appdata_ptr)[0];
                (*resp)[i].resp = strdup(username);
                break;
            case PAM_PROMPT_ECHO_OFF:
                password = ((char **) appdata_ptr)[1];
                (*resp)[i].resp = strdup(password);
                break;
            case PAM_ERROR_MSG:
                fprintf(stderr, "%s\n", msg[i]->msg);
                result=PAM_CONV_ERR;
                break;
            case PAM_TEXT_INFO:
                printf("%s\n", msg[i]->msg);
                break;
        }
        if (result != PAM_SUCCESS) {
            break;
        }
    }
    if (result != PAM_SUCCESS) {
        free(*resp);
        *resp=0;
    }
    return result;
}

static void init_env(struct passwd *pw) {
    set_env("HOME", pw->pw_dir);
    set_env("PWD", pw->pw_dir);
    set_env("SHELL", pw->pw_shell);
    set_env("USER", pw->pw_name);
    set_env("LOGNAME", pw->pw_name);
    set_env("PATH", "/usr/local/sbin:/usr/local/bin:/usr/bin");
    set_env("MAIL", _PATH_MAILDIR);

    size_t xauthority_len = strlen(pw->pw_dir) + strlen("/.Xauthority") + 1;
    char *xauthority = malloc(xauthority_len);
    snprintf(xauthority, xauthority_len, "%s/.Xauthority", pw->pw_dir);
    set_env("XAUTHORITY", xauthority);
    setenv("XAUTHORITY", xauthority, 1);
    free(xauthority);
}

static void set_env(char *name, char *value) {
    // plus 2 to account for the equal + null terminal bytes
    size_t name_value_len = strlen(name) + strlen(value) + 2;
    char *name_value = malloc(name_value_len);
    snprintf(name_value, name_value_len, "%s=%s", name, value);
    int result = pam_putenv(pam_handle, name_value);
    if (result != PAM_SUCCESS) {
        printf("FATAL ENV ERROR");
        exit(1);
    }
    free(name_value);
}

bool logout() {
    int result = pam_close_session(pam_handle, 0);
    if (pam_err(result, "pam_close_session")) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        return false;
    }

    result = pam_setcred(pam_handle, PAM_DELETE_CRED);
    if (pam_err(result, "pam_setcred")) {
        return false;
    }

    end(result);
    return true;
}

static int end(int last_result) {
    int result = pam_end(pam_handle, last_result);
    pam_handle = 0;
    return result;
}
