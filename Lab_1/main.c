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

#define COLOR_DIR "\x1b[34m"   // Синий
#define COLOR_EXEC "\x1b[32m"  // Зеленый
#define COLOR_LINK "\x1b[36m"  // Голубой
#define COLOR_RESET "\x1b[0m"  // Сброс цвета

int compareEntries(const void *a, const void *b) {
    const char *nameA = *(const char **)a;
    const char *nameB = *(const char **)b;

    // Сначала выводим файлы, начинающиеся с точки
    if (nameA[0] == '.' && nameB[0] != '.') {
        return -1;
    }
    if (nameA[0] != '.' && nameB[0] == '.') {
        return 1;
    }

    // Сравнение с учетом регистра
    return strcasecmp(nameA, nameB);
}

void printColor(const char *name, struct stat fileStat, const char *fullPath) {
    if (S_ISDIR(fileStat.st_mode)) {
        printf(COLOR_DIR "%-20s" COLOR_RESET, name);
    } else if (fileStat.st_mode & S_IXUSR) {
        printf(COLOR_EXEC "%-20s" COLOR_RESET, name);
    } else if (S_ISLNK(fileStat.st_mode)) {
        char linkTarget[1024];
        ssize_t len = readlink(fullPath, linkTarget, sizeof(linkTarget) - 1);
        if (len != -1) {
            linkTarget[len] = '\0';
            printf(COLOR_LINK "%-20s" COLOR_RESET " -> %s", name, linkTarget);
        } else {
            printf(COLOR_LINK "%-20s" COLOR_RESET, name);
        }
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
    long long totalBlocks = 0;

    // Считываем файлы
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

    // Вывод общего количества блоков в длинном формате
    if (longFormat) {
        for (int i = 0; i < count; i++) {
            struct stat fileStat;
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, entries[i]);
            if (lstat(fullPath, &fileStat) < 0) {
                perror("stat");
                continue;
            }
            totalBlocks += fileStat.st_blocks;
        }
        printf("total %lld\n", totalBlocks / 2);  // Вывод общего количества блоков
    }

    // Вывод файлов
    for (int i = 0; i < count; i++) {
        struct stat fileStat;
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, entries[i]);
        if (lstat(fullPath, &fileStat) < 0) {
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

            // Вывод времени
            char timebuf[100];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&fileStat.st_mtime));
            printf(" %s ", timebuf); 

            // Вывод имени файла с цветом и обработкой символических ссылок
            printColor(entries[i], fileStat, fullPath);
            printf("\n");
        } else {
            printColor(entries[i], fileStat, fullPath);
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
