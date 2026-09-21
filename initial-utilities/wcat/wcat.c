#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        exit(0);
    }
    for (int i = 1; i < argc; i++) {
        char *file = argv[i];
        FILE *fp = fopen(file, "r");
        if (fp == NULL) {
          printf("wcat: cannot open file\n");
            exit(1);
        }

        char line[1024];

        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
    }
    exit(0);
}