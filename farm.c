#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s ./farm [masterWorker arguments...]\n", argv[0]);
        exit(1);
    }

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("Fork failed");
        exit(1);
    } else if (child_pid == 0) {  // Processo figlio
        execvp("./masterWorker", argv);
        perror("Exec failed");
        exit(1);
    } else {  // Processo principale
         execv("./collector", 0);
    }

    return 0;
}
