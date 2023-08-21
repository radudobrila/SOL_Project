// Progetto di Sistemi Operativi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char *argv[]){
    int mw_pipe[2];
    
    if( ( pipe(mw_pipe) == -1)){
        perror("Error: Pipe_Creation_Failed\n");
        return -1;
    }

    pid_t masterWorker_pid = fork();

    if(masterWorker_pid == -1){
        perror("Error: Fork_Error (MasterWorker)\n");
        return -1;
    }
    else if(masterWorker_pid == 0){
        // Processo figlio MasterWorker

        close(mw_pipe[1]);
        dup2(mw_pipe[0], STDIN_FILENO);
        execl("./MasterWorker", "MasterWorker", NULL);
        perror("Exec (MasterWorker) failed");
        return -1;
    }
    else{
        // Processo padre Main
        close(mw_pipe[0]);

        write(mw_pipe[1], argv, argc + 1);            

        wait(NULL);
    }

    return 0;
    
}