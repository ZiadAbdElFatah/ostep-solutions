#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_found(char *line, int line_size, char *search, int search_size);
void grep_file(FILE *fp, char *search);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }

    char *search = argv[1];

    if (argc == 2) {
        grep_file(stdin, search);
    }

    for (int i = 2; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");

        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            exit(1);
        }

        grep_file(fp, search);

        fclose(fp);
    }

    return 0;
}

void grep_file(FILE *fp, char *search) {
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1) {
        if (is_found(line, strlen(line), search, strlen(search))) {
            printf("%s", line);
        }
    }

    free(line);
}

int is_found(char *line, int line_size, char *search, int search_size) {
    int found = 0;

    for (int i = 0; i < line_size; i++) {
        found = 0;

        if (line[i] == search[0]) {
            int j = 0;

            while (j < search_size) {
                if (line[i + j] != search[j]) {
                    break;
                }

                j++;
            }

            if (j == search_size) {
                found = 1;
                break;
            }
        }
    }

    return found;
}