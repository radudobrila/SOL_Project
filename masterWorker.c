#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <util.h>
#include <getopt.h>
#include "includes/threadpool.h"

#define ext ".dat"
#define MAX_PATH_NAME 255

void * worker(void *arg){
    FILE *fp;
    char *filename = (char *)arg;
    int client_socket;
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if(client_socket == -1){
        perror("Error: Socket_Creation_Error\n");
        return NULL;
    }

    struct sockaddr_un client_address;
    memset(&client_address, 0, sizeof(struct sockaddr_un));
    client_address.sun_family = AF_UNIX;
    strcpy(client_address.sun_path, "./farm.sck");


    fp = fopen(filename, "rb");

    if(fp == NULL){
        perror("Error: Open_File_Error\n");
        return NULL;
        return NULL;
    }

    long total_sum = 0;
    long value = 0;
    long riga = 1;
    while (fread(&value, sizeof(long), 1, fp) == 1) {
    if (ferror(fp)) {
        perror("Error: Reading from file\n");
        fclose(fp);
        return NULL; 
    }
    total_sum += value * riga;
    riga++;
}

    //printf("(Client): La somma dei valori da inviare nel file '%s' è: %ld\n",(char *) arg, total_sum);

    if(connect(client_socket, (struct sockaddr *) &client_address, sizeof(client_address)) == -1){
        perror("Socket_Connection_Error\n");
        return NULL;
    }
    //printf("(Client): Connesso al Server...\n");
    //printf("CLIENT: Sto per mandare %s\n\n", arg);
    ssize_t bytes_sent = writen(client_socket, (char*) arg, 255);
    if (bytes_sent == -1) {
        perror("Socket_Send_Error\n");
        return NULL;
    }

    bytes_sent = writen(client_socket, &total_sum, sizeof(total_sum));
    if (bytes_sent == -1) {
        perror("Socket_Send_Error\n");
        return NULL;
    }

    //printf("(Client): Scritto.\n");
    fflush(stdout);

    close(client_socket);

    fflush(stdout);
    fclose(fp);
	//free(filename);

    return 0;
}

void traverseDirectory(const char *path, threadpool_t *myPool) {
    DIR *dir = opendir(path);
    
    if (!dir) {
        perror("Error: Open_Directory\n");
        return;
    }

    struct dirent *entry;
	char *entryPath = NULL;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            entryPath = calloc(MAX_PATH_NAME, sizeof(char));
			strncat(entryPath, path, strnlen(path, MAX_PATH_NAME));
			strncat(entryPath, "/", 2);
			strncat(entryPath, entry->d_name, strnlen(entry->d_name, MAX_PATH_NAME));
            traverseDirectory(entryPath, myPool);
			free(entryPath);
        } else if (entry->d_type == DT_REG) {
            size_t len = strlen(entry->d_name);
            size_t ext_len = strlen(ext);

            if (len > ext_len && strcmp(entry->d_name + len - ext_len, ext) == 0) {
                // Processa il file .dat qui            
				entryPath = calloc(MAX_PATH_NAME, sizeof(char));
				strncat(entryPath, path, strnlen(path, MAX_PATH_NAME));
				strncat(entryPath, "/", 2);
				strncat(entryPath, entry->d_name, strnlen(entry->d_name, MAX_PATH_NAME));
                int x = addToThreadPool(myPool, (void*) worker, entryPath);
                if(x == 1){
                    printf("Qua è piena\n");
                }
            }
        }
    }

    closedir(dir);
}

// funzione che prende l'argomento delle opzioni (n,q e t) e ritorna un intero
int arg_int(const char *n){
    int tmp;
    tmp = atoi(n);
    return tmp;
}



int main(int argc, char *argv[]){
    if(argc <= 1){
        perror("Error: Invalid_Argument\n");
        return -1;
    }

    int opt , n = 2, q = 5, flag = 0;
    char directory[256];
    
    while((opt = getopt(argc, argv, "n:q:d:t:")) != -1){
        switch (opt)
        {
        case 'n': 
            n = arg_int(optarg);
            printf("N = %d\n", n);
        break;
        case 'q':
            q = arg_int(optarg);
            printf("q = %d\n",q);
        break;
        case 'd':
            strcpy(directory,optarg);
            flag = 1;
        break;
        case 't':
            printf("t = %d\n",arg_int(optarg));
        break;
        case ':': {
                printf("l'opzione -%c richiede un argomento\n", optopt);}
                return 0;
        break;
        case '?': {
                printf("l'opzione -%c non e' gestita\n", optopt);}
                return 0;
        break;

        default:
            break;
        }
    }

    pid_t pid = fork();

    if(pid == 0){
        execv("./collector", 0);
    }

    sleep(1);

    threadpool_t *myPool = createThreadPool(n, q); 


    for (int i = optind; i < argc; i++) {
        const char *filename = argv[i];
        size_t len = strlen(filename);
        size_t ext_len = strlen(ext);

        if (len > ext_len && strcmp(filename + len - ext_len, ext) == 0) {
            int x = addToThreadPool(myPool,(void *) worker, argv[i]);
        }
    }

    if(flag != 0) {
        //printf("Nome Dir: %s\n", directory);
        traverseDirectory(directory, myPool);
    }
    
	sleep(5);
    destroyThreadPool(myPool, 0);
        
    return 0;
}