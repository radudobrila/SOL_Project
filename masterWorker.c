#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <getopt.h>
#include "includes/threadpool.h"

#define ext ".dat"
#define MAX_PATH_NAME 255

void *printName(void *arg){
    char *message = arg; // Devo aggiungere qui le task
    printf("%s\n", message);
    return NULL;
}

void traverseDirectory(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Error: Open_Directory\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char entryPath[MAX_PATH_NAME];
        snprintf(entryPath, sizeof(entryPath), "%s/%s", path, entry->d_name);

        struct stat entryStat;
        if (stat(entryPath, &entryStat) == 0) {
            if (S_ISDIR(entryStat.st_mode)) {
                // Se l'elemento è una directory, richiama la funzione ricorsivamente
                traverseDirectory(entryPath);
            } else {
                // Altrimenti, è un file
                printf("File: %s\n", entryPath);
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

// funzione che prende l'argomento dell'opzione d e controlla che esista la cartella
int arg_d(const char *n){
    const char *startPath = n;
    traverseDirectory(startPath);
    return 0;
}



int main(int argc, char *argv[]){
    if(argc <= 1){
        perror("Error: Invalid_Argument\n");
        return -1;
    }

    int opt, n, q;
    char *directory;
    
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
            directory = optarg;
            printf("%s\n", directory);
        break;
        case 't':
            printf("t = %d\n",arg_int(optarg));
        break;
        case ':': {
                printf("l'opzione -%c richiede un argomento\n", optopt);}
        break;
        case '?': {
                printf("l'opzione -%c non e' gestita\n", optopt);}
        break;

        default:
            break;
        }
    }
    printf("Creo ThreadPool\n");
    threadpool_t *myPool = createThreadPool(n, q); 
    printf("Threadpool Creata\n");
    printf("Numero di thread: %d\n", myPool->numthreads);

    for (int i = optind; i < argc; i++) {
        const char *filename = argv[i];
        size_t len = strlen(filename);
        size_t ext_len = strlen(ext);

        if (len > ext_len && strcmp(filename + len - ext_len, ext) == 0) {
            addToThreadPool(myPool, printName, argv[i]);
        }
    }

    printf("Task da eseguire = %d\n", myPool->count);
    printf("Prima di Join\n");
    for (int t = 0; t < myPool->numthreads; t++) {
        pthread_join(myPool->threads[t], NULL);
        destroyThreadPool(myPool, 0);
    }

    
    return 0;
}