#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    char last_char;
    int cnt = 0;

    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wzip: cannot open file\n");
            exit(1);
        }
        char *line = NULL;
        size_t len = 0;

        while (getline(&line, &len, fp) != -1) {
            for (int i = 0; i < strlen(line); i++) {
                if (cnt == 0) {
                    last_char = line[i];
                    cnt = 1;
                } else if (line[i] == last_char) {
                    cnt++;
                } else {
                    fwrite(&cnt, sizeof(cnt), 1, stdout);
                    fwrite(&last_char, sizeof(last_char), 1, stdout);
                    last_char = line[i];
                    cnt = 1;
                }
            }
        }
        fclose(fp);
    }
    fwrite(&cnt, sizeof(cnt), 1, stdout);
    fwrite(&last_char, sizeof(last_char), 1, stdout);
    exit(0);
}