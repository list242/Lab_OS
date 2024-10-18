#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void exit_handler() {
    printf("Программа завершает работу.\n");
}
void sigint_handler(int signum) {
    printf("Получен сигнал SIGINT (%d)\n", signum);
}
void sigterm_handler(int signum) {
    printf("Получен сигнал SIGTERM (%d)\n", signum);
}

int main(){
    signal(SIGINT, sigint_handler);
    struct sigaction sa;
    sa.sa_handler = sigterm_handler;
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    atexit(exit_handler);
    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при вызове fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Это дочерний процесс с PID: %d\n", getpid());
        sleep(2);
        printf("Дочерний процесс завершает работу.\n");
        exit(0);
    } else {
        printf("Это родительский процесс с PID: %d, дочерний PID: %d\n", getpid(), pid);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился с кодом: %d\n", WEXITSTATUS(status));
        }
    }
    printf("Родительский процесс ожидает сигналы...\n");
    while (1) {
        pause();
    }
    return 0;
}
