#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void add_lines(char ***output, int *size, int *capacity, FILE *fp);
void write_lines(char **output, int size, FILE *fp);
int same_file(const char *path1, const char *path2);

int main(int argc, char *argv[]) {
    FILE *fp;
    char **output;
    int size = 0, capacity = 4;
    output = malloc(capacity * sizeof(char *));
    if (argc == 1)
    {
        fp = stdin;
        add_lines(&output, &size, &capacity, stdin);   
    } else if (argc == 2 || argc == 3) {
        if (argc == 3)
        {
            if (same_file(argv[1], argv[2]))
            {
                fprintf(stderr, "reverse: input and output file must differ\n");
                exit(1);
            }
        }
        
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }

        add_lines(&output, &size, &capacity, fp);
    } else {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if (fp != stdin)
    {
        fclose(fp);
    }

    if (argc < 3)
    {
        write_lines(output, size, stdout);
    } else {
        fp = fopen(argv[2], "w");
        if (fp == NULL)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
        write_lines(output, size, fp);
        if (fp != stdin) {
            fclose(fp);
        }
    }
}

void add_lines(char ***output, int *size, int *capacity, FILE *fp) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1)
    {
        if (*size == *capacity)
        {
            *output = realloc(*output, (*capacity) * sizeof(char *));
            *capacity *= 2;
        }
        (*output)[(*size)++] = strdup(line);
    }
}

void write_lines(char **output, int size, FILE *fp) {
    for (int i = size - 1; i >= 0; i--)
    {
        fprintf(fp, "%s", output[i]);
    }
}

int same_file(const char *path1, const char *path2) {
    struct stat st1, st2;
    if (stat(path1, &st1) != 0) return 0;
    if (stat(path2, &st2) != 0) return 0;
    return (st1.st_dev == st2.st_dev) && (st1.st_ino == st2.st_ino);
}