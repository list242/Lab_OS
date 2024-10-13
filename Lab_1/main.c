#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef __unix__
#include <pwd.h>
#include <grp.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#endif

int compareEntries(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void printColor(const char *name, struct stat fileStat) {
    if (S_ISDIR(fileStat.st_mode)) {
        printf("\033[34m%-20s\033[0m", name);
    } else if (fileStat.st_mode & S_IXUSR) {
        printf("\033[32m%-20s\033[0m", name);
    } else if (S_ISLNK(fileStat.st_mode)) {
        printf("\033[36m%-20s\033[0m", name);
    } else {
        printf("%-20s", name);
    }
}

void listFiles(const char *directory, int showHidden, int longFormat) {
#ifdef __unix__
    struct dirent *entry;
    DIR *dp = opendir(directory);
    if (dp == NULL) {
        perror("opendir");
        return;
    }
    char *entries[1024];
    int count = 0;

    while ((entry = readdir(dp))) {
        if (!showHidden && entry->d_name[0] == '.') {
            continue;
        }
        if (count < 1024) {
            entries[count++] = strdup(entry->d_name);
        } else {
            fprintf(stderr, "Too many files in directory!\n");
            break;
        }
    }
    closedir(dp);
    qsort(entries, count, sizeof(char *), compareEntries);

    for (int i = 0; i < count; i++) {
        struct stat fileStat;
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, entries[i]);
        if (stat(fullPath, &fileStat) < 0) {
            perror("stat");
            continue;
        }
        if (longFormat) {
           // Вывод прав доступа
            printf("%c", (S_ISDIR(fileStat.st_mode) ? 'd' : (S_ISLNK(fileStat.st_mode) ? 'l' : '-')));
            printf("%c%c%c", 
                (fileStat.st_mode & S_IRUSR) ? 'r' : '-', 
                (fileStat.st_mode & S_IWUSR) ? 'w' : '-', 
                (fileStat.st_mode & S_IXUSR) ? 'x' : '-'); 
            printf("%c%c%c", 
                (fileStat.st_mode & S_IRGRP) ? 'r' : '-',
                (fileStat.st_mode & S_IWGRP) ? 'w' : '-',
                (fileStat.st_mode & S_IXGRP) ? 'x' : '-');
            printf("%c%c%c", 
                (fileStat.st_mode & S_IROTH) ? 'r' : '-', 
                (fileStat.st_mode & S_IWOTH) ? 'w' : '-', 
                (fileStat.st_mode & S_IXOTH) ? 'x' : '-'); 
            printf(" %2ld", (long)fileStat.st_nlink); 
            printf(" %-8s %-8s", getpwuid(fileStat.st_uid)->pw_name, getgrgid(fileStat.st_gid)->gr_name);
            printf(" %8lld", (long long)fileStat.st_size);

            char timebuf[100];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&fileStat.st_mtime));
            printf(" %s", timebuf); 
            printf(" %s\n", entries[i]); 
        } else {
            printColor(entries[i], fileStat);
            printf("\n");
        }
    }
    for (int i = 0; i < count; i++) {
        free(entries[i]);
    }
#else
    fprintf(stderr, "This program is only supported on UNIX systems.\n");
#endif
}

int main(int argc, char *argv[]) {
    int showHidden = 0;
    int longFormat = 0;
    int opt;
    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt) {
            case 'l':
                longFormat = 1;
                break;
            case 'a':
                showHidden = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-a] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    const char *directory = (optind < argc) ? argv[optind] : ".";
    listFiles(directory, showHidden, longFormat);
    return 0;
}
