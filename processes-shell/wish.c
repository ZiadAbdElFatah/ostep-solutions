#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void error();
int find_app(char **cmd, char *dir, char **path);
int tokenize(char *s, char **out, int max);
int run_segment(char *seg);
void run_cmd(FILE *fp);
void cd(char* dir);

char *search_paths[32];
int num_paths = 0;

int main(int argc, char *argv[]) {
    // batch
    if (argc > 1) {
        if (argc > 2) {
            error();
            exit(1);
        }
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            error();
            exit(1);
        }
        run_cmd(fp);
    } else {
        printf("wish> ");
        run_cmd(stdin);
    }
}

void error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

int find_app(char **cmd, char *dir, char **path) {
    static char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir, *cmd);
    if (access(full_path, X_OK) == 0) {
        *path = full_path;
        return 1;
    }
    return 0;
}

int tokenize(char *s, char **out, int max) {
    int i = 0;
    char *tok;
    while ((tok = strsep(&s, " \t"))) {
        if (strcmp(tok, "") == 0) {
            continue;
        }
        out[i++] = tok;
        if (i == max) {
            return -1;
        }
    }
    out[i] = NULL;
    return i;
}

int run_segment(char *seg) {
    char *cmd_args[30];
    char *redir_args[3];
    char *redir_file = NULL;

    char *p = seg;
    char *left = strsep(&p, ">");
    char *right = p;

    int no_args = tokenize(left, cmd_args, 30);

    if (no_args < 0) {
        error();
        return 0;
    }

    if (no_args == 0) {
        if (right != NULL) {
            error();
        }
        return 0;
    }

    if (right != NULL) {
        if (strchr(right, '>') != NULL) {
            error();
            return 0;
        }

        if (tokenize(right, redir_args, 2) != 1) {
            error();
            return 0;
        }
        redir_file = redir_args[0];
    }

    // exit
    if (strcmp(cmd_args[0], "exit") == 0) {
        if (no_args != 1) {
            error();
            return 0;
        }
        exit(0);
    }
    // cd
    if (strcmp(cmd_args[0], "cd") == 0) {
        if (no_args != 2) {
            error();
        } else {
            cd(cmd_args[1]);
        }
        return 0;
    }
    // path
    else if (strcmp(cmd_args[0], "path") == 0) {
        if (no_args - 1 > 32) {
            error();
            return 0;
        }
        for (int i = 0; i < num_paths; i++) {
            free(search_paths[i]);
        }
        num_paths = 0;
        int i = 1;
        while (cmd_args[i] != NULL) {
            search_paths[i - 1] = strdup(cmd_args[i]);
            i++;
        }
        num_paths = i - 1;
        return 0;
    }

    int found = 0;
    char *path;
    for (int i = 0; i < num_paths; i++) {
        found = find_app(&cmd_args[0], search_paths[i], &path);
        if (found) {
            break;
        }
    }
    if (!found) {
        error();
        return 0;
    }

    int rc = fork();

    if (rc == 0) {
        if (redir_file != NULL) {
            close(STDOUT_FILENO);
            int fd = open(redir_file, O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (fd < 0) {
                error();
                exit(1);
            }
        }

        execv(path, cmd_args);
        error();
        exit(1);
    } else if (rc < 0) {
        error();
        return 0;
    }
    return 1;
}

void run_cmd(FILE *fp) {
    search_paths[0] = strdup("/bin");
    num_paths = 1;
    char *line = NULL;
    size_t sz = 0;
    while (getline(&line, &sz, fp) != -1) {
        line[strcspn(line, "\n")] = '\0';

        int started = 0;
        char *p = line;
        char *seg;
        while ((seg = strsep(&p, "&")) != NULL) {
            started += run_segment(seg);
        }

        for (int i = 0; i < started; i++) {
            wait(NULL);
        }

        if (fp == stdin) {
            printf("wish> ");
        }
    }
    exit(0);
}

void cd(char *dir) {
    if (chdir(dir) != 0) {
        error();
    }
}