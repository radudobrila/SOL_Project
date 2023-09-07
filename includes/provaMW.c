#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <getopt.h>
#include "includes/threadpool.h"

#define ext ".dat"
#define MAX_PATH_NAME 255

void * worker(void *arg){
    FILE *fp;
    const char *filename = (const char *)arg;
    printf("QUI ENTRANO: %s\n", filename);
    fp = fopen(filename, "rb");

    if(fp == NULL){
        perror("Error: Open_File_Error\n");
        return NULL;
        return NULL;
    }

    int total_sum = 0;
    int value;
    
    while (fread(&value, sizeof(int), 1, fp) == 1) {
    if (ferror(fp)) {
        perror("Error: Reading from file\n");
        fclose(fp);
        return NULL; 
    }
    total_sum += value;
}

    printf("La somma dei valori nel file '%s' è: %d\n",(char *) arg, total_sum);
    fflush(stdout);
    fclose(fp);


    return 0;
}

void traverseDirectory(const char *path, threadpool_t myPool) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Error: Open_Directory\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char entryPath[PATH_MAX];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", path, entry->d_name);
            traverseDirectory(entryPath);
        } else if (entry->d_type == DT_REG) {
            size_t len = strlen(entry->d_name);
            size_t ext_len = strlen(ext);

            if (len > ext_len && strcmp(entry->d_name + len - ext_len, ext) == 0) {
                // Processa il file .dat qui
                char entryPath[PATH_MAX];
                snprintf(entryPath, sizeof(entryPath), "%s/%s", path, entry->d_name);
                printf("File .dat trovato: %s\n", entryPath);
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

/*
// funzione che prende l'argomento dell'opzione d e controlla che esista la cartella
int arg_d(const char *n){
    const char *startPath = n;
    traverseDirectory(startPath, );
    return 0;
}
*/


int main(int argc, char *argv[]){
    if(argc <= 1){
        perror("Error: Invalid_Argument\n");
        return -1;
    }

    int opt, n = 15, q = 15, flag = 0;
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
            flag = 1;
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

    threadpool_t *myPool = createThreadPool(n, q); 

    for (int i = optind; i < argc; i++) {
        const char *filename = argv[i];
        size_t len = strlen(filename);
        size_t ext_len = strlen(ext);

        if (len > ext_len && strcmp(filename + len - ext_len, ext) == 0) {
            addToThreadPool(myPool,(void *) worker, argv[i]);
        }
    }

    if(flag != 0) {
        printf("Nome Dir: %s\n", directory);
        traverseDirectory(directory, myPool);
    }
 

    printf("Num THREAD: %d\n", myPool->numthreads);
    for(int t = 0; t < myPool->numthreads; t++){
        pthread_join(myPool->threads[t], NULL);
    }

    return 0;
}