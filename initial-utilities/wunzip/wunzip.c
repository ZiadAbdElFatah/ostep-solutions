#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2)
    {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    char letter;
    int cnt;

    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wunzip: cannot open file\n");
            exit(1);
        }

        while (fread(&cnt, sizeof(cnt), 1, fp) == 1)
        {
            fread(&letter, sizeof(letter), 1, fp);
            for (int i = 0; i < cnt; i++) {
                printf("%c", letter);
            }
        }
        
        fclose(fp);
    }
    exit(0);
}