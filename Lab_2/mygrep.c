#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void grep_pattern(FILE *file, const char *pattern) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strstr(buffer, pattern)) {
            printf("%s", buffer);
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

int main(int argc, char *argv[]) {
    const char *pattern;
    char *filename = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pattern> [file]\n", argv[0]);
        return 1;
    }

    pattern = argv[1];
    if (argc > 2) {
        filename = argv[2];
    }

    FILE *file = open_file(filename);
    grep_pattern(file, pattern);

    if (filename) {
        fclose(file);
    }

    return 0;
}

