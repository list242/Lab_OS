#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Line_numb(FILE *file) { 
    char buffer[1024];
    int line_numb = 1;
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%d %s", line_numb++, buffer);
    }
}

void Deline_numb(FILE *file) {
    char buffer[1024];
    int line_numb = 1;
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0) {
            printf("%d %s\n", line_numb++, buffer);
        }
    }
}

void dollar_add(FILE *file) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s$\n", buffer);
    }
}

void parse_arguments(int argc, char *argv[], int *flag_n, int *flag_b, int *flag_E, const char **pattern, char **filename) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) { 
            *flag_n = 1;
        } else if (strcmp(argv[i], "-b") == 0) {
            *flag_b = 1;
        } else if (strcmp(argv[i], "-E") == 0) {
            *flag_E = 1;
        } else { 
            *pattern = argv[i];
            if (i + 1 < argc) {
                *filename = argv[i + 1];
            }
        }
    }
}

FILE* open_file(const char *filename) {
    FILE *file;
    if (filename) {
        file = fopen(filename, "r");
        if (!file) {
            perror("Error opening file");
            exit(1);
        }
    } else {
        file = stdin;
    }
    return file;
}

void process_flags(FILE *file, int flag_n, int flag_b, int flag_E, const char *pattern) {
    if (flag_n && flag_E) {
        char buffer[1024];
        int line_numb = 1;
        while (fgets(buffer, sizeof(buffer), file)) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strstr(buffer, pattern)) {
                printf("%d %s$\n", line_numb++, buffer);
            }
        }
    } else if (flag_b && flag_E) {
        char buffer[1024];
        int line_numb = 1;
        while (fgets(buffer, sizeof(buffer), file)) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strstr(buffer, pattern) && strlen(buffer) > 0) {
                printf("%d %s$\n", line_numb++, buffer);
            } else if (strstr(buffer, pattern)) {
                printf("%s$\n", buffer);
            }
        }
    } else if (flag_n) {
        Line_numb(file);  
    } else if (flag_b) {
        Deline_numb(file);
    } else if (flag_E) {
        dollar_add(file);
    } else {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file)) {
            if (strstr(buffer, pattern)) {
                printf("%s", buffer);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int flag_n = 0;
    int flag_b = 0; 
    int flag_E = 0; 
    const char *pattern = NULL;
    char *filename = NULL;
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-n|-b|-E] <pattern> [file]\n", argv[0]);
        return 1;
    }
    parse_arguments(argc, argv, &flag_n, &flag_b, &flag_E, &pattern, &filename);

    FILE *file = open_file(filename);
    process_flags(file, flag_n, flag_b, flag_E, pattern);
    if (filename) {
        fclose(file);
    }
    return 0;
}
