#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct entry
{
    int key; char *value;
};

void load_file(FILE *fp, struct entry **entries, int *count, int *capacity);
void insert(struct entry **entries, int *count, int *capacity, int key, char *value);
void search_file(struct entry *entries, int size, int search_key);
void print_all(struct entry *entries, int size);
void delete(struct entry **entries, int *count, int key);
void write_to_file(FILE *fp, struct entry *entries, int size);

int main(int argc, char *argv[]) {
    int key; char *value;
    struct entry *entries;

    FILE *fp = fopen("database.txt", "r");
    int entry_count = 0, entries_capacity = 4;
    if (fp != NULL) {
        load_file(fp, &entries, &entry_count, &entries_capacity);
        fclose(fp);
    } else {
        entries = malloc(entries_capacity * sizeof(struct entry));
    }

    fp = fopen("database.txt", "w");
    if (fp == NULL)
    {
        printf("kv can't open file\n");
        exit(1);
    }
    

    for (int i = 1; i < argc; i++)
    {
        char *line = argv[i];
        char *cmd   = strsep(&line, ",");
        char *input_key = strsep(&line, ",");
        char *input_value  = strsep(&line, ",");

        if (cmd[0] == 'p')
        {
            key = atoi(input_key); value = input_value;
            insert(&entries, &entry_count, &entries_capacity, key, value);
        } 
        else if (cmd[0] == 'g')
        {
            key = atoi(input_key);
            search_file(entries, entry_count, key);
        } 
        else if (cmd[0] == 'a')
        {
            print_all(entries, entry_count);
        } 
        else if (cmd[0] == 'c')
        {
            for (int i = 0; i < entry_count; i++)
            {
                free(entries[i].value);
            }
            entry_count = 0;
            entries_capacity = 4;
            free(entries);
            entries = malloc(entries_capacity * sizeof(struct entry));
        } 
        else if (cmd[0] == 'd')
        {
            key = atoi(input_key);
            delete(&entries, &entry_count, key);
        } 
        else {
            printf("bad command\n");
        }
        
    }
    
    write_to_file(fp, entries, entry_count);

    fclose(fp);
}

void load_file(FILE *fp, struct entry **entries, int *count, int *capacity) {
    *count = 0;
    *capacity = 4;
    *entries = malloc(*capacity * sizeof(struct entry));

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1) {
        char *keystr = strsep(&line, ",");
        char *valstr = strsep(&line, ",");
        valstr[strcspn(valstr, "\n")] = '\0';

        if (*count == *capacity) {
            *capacity *= 2;
            *entries = realloc(*entries, *capacity * sizeof(struct entry));
        }

        (*entries)[*count].key = atoi(keystr);
        (*entries)[*count].value = strdup(valstr);
        (*count)++;
    }
    free(line);
}

void insert(struct entry **entries, int *count, int *capacity, int key, char *value) {
    for (int i = 0; i < *count; i++)
    {
        if ((*entries)[i].key == key)
        {
            free((*entries)[i].value);
            (*entries)[i].value = strdup(value);
            return;
        }
    }
    
    if (*count == *capacity)
    {
        *capacity *= 2;
        *entries = realloc(*entries, *capacity * sizeof(struct entry));
    }
    
    (*entries)[*count].key = key;
    (*entries)[*count].value = strdup(value);
    (*count)++;
}

void search_file(struct entry *entries, int size, int search_key) {
    for (int i = 0; i < size; i++)
    {
        if (entries[i].key == search_key)
        {
            printf("%d,%s\n", entries[i].key, entries[i].value);
            return;
        }
    }

    printf("%d not found\n", search_key);
}

void print_all(struct entry *entries, int size) {
    for (int i = 0; i < size; i++)
    {
        printf("%d,%s\n", entries[i].key, entries[i].value);
    }
}

void delete(struct entry **entries, int *count, int key) {
    for (int i = 0; i < *count; i++)
    {
        if ((*entries)[i].key == key)
        {
            (*entries)[i] = (*entries)[*count - 1];
            free((*entries)[*count - 1].value);
            (*count)--;
            return;
        }
    }
}

void write_to_file(FILE *fp, struct entry *entries, int size) {
    for (int i = 0; i < size; i++)
    {
        fprintf(fp, "%d,%s\n", entries[i].key, entries[i].value);
    }
}
