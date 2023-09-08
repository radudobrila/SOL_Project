#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <getopt.h>
#include "includes/threadpool.h"
#include <util.h>

#define SOCKET_MAX_LEN 1024

typedef struct orderedList{
    char path[255];
    long val;
    struct orderedList *prev;
    struct orderedList *next;
}orderedList;

void freeOrderedList(orderedList *myList) {
    while (myList != NULL) {
        orderedList *temp = myList;
        myList = myList->next;
        free(temp);
    }
}

void printOrderedList(orderedList *myList) {
    printf("Ordered List:\n");
    while (myList != NULL) {
        printf("Value: %ld, Path: %s\n", myList->val, myList->path);
        myList = myList->next;
    }
}

void addSorted(orderedList **myList, orderedList *nuovo) {
    orderedList *attuale = *myList;

    if (*myList == NULL) {
        nuovo->next = NULL;
        nuovo->prev = NULL;
        *myList = nuovo;
        return; 
    }

    while (attuale != NULL) {
        if (attuale->val > nuovo->val) {
            nuovo->prev = attuale->prev;
            nuovo->next = attuale;
            attuale->prev = nuovo;

            if (nuovo->prev != NULL) {
                nuovo->prev->next = nuovo;
            } else {
                *myList = nuovo; 
            }
            return; 
        }
        if (attuale->next == NULL) {
            attuale->next = nuovo;
            nuovo->prev = attuale;
            nuovo->next = NULL;
            return; 
        }
        attuale = attuale->next;
    }
}

int main(int argc, char *argv[]){
    orderedList *MyList;
    int server_socket;
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if(server_socket == -1){
        perror("Error: Socket_creation_Error\n");
        return -1;
    }

    struct sockaddr_un serverAddress;
    serverAddress.sun_family = AF_UNIX;
    strcpy(serverAddress.sun_path, "./farm.sck");

    if(bind(server_socket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1){
        perror("Socket_Bind_Error\n");
        return -1;
    }

    if( listen(server_socket, 10) == -1){
        perror("Socket_Listen_Error\n");
        return -1;
    }

    //printf("(Server): Server in ascolto...\n");

    int max_fd = server_socket;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);
    int i = 0;
    struct timeval tv = {3, 0};   
    int ret;
    while (1) {
        fd_set temp_fds = read_fds;
        if ((ret = select(max_fd + 1, &temp_fds, NULL, NULL, &tv))== -1) {
            perror("Select_Error\n");
            return -1;
        } else if(ret == 0) break;

        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &temp_fds)) {
                if (fd == server_socket) { // Nuova connessione in arrivo
                    int new_socket = accept(server_socket, NULL, 0);
                    if (new_socket == -1) {
                        perror("Accept_Error\n");
                    } else {
                        FD_SET(new_socket, &read_fds);
                        if (new_socket > max_fd) {
                            max_fd = new_socket;
                        }
                        //printf("(Server): Nuova connessione accettata...\n");
                    }
                } else { 
                    //printf("(SERVER): Sono dentro la connessione socket\n");
                    char path[255];
                    orderedList *nuovo = malloc(sizeof(orderedList));
                    ssize_t bytes_read = readn(fd, path, 255);
                    long val = 0;
                    if (bytes_read == -1) {
                        perror("Socket_Read_Error\n");
                        return -1;
                    }

                    bytes_read = readn(fd, &val, sizeof(long));
                    
                    if (bytes_read == -1) {
                        perror("Socket_Read_Error\n");
                        return -1;
                    }
                    
                    strcpy(nuovo->path, path);
                    nuovo->val = val;
                    //printf("AGGIUNGO: %s\n", nuovo->path);
                    addSorted(&MyList, nuovo);
                    //printf("(SERVER): Letto: P: %s \n V: %ld\n", nuovo->path, nuovo->val);
                    
                    //fflush(stdout);
                    FD_CLR(fd, &read_fds);
                    close(fd);
                    i++;
                   //printf("(Server): Connessione chiusa...\n");
                    
                }
            }
        }
    }
    
    printOrderedList(MyList);
    close(server_socket);
    unlink("./farm.sck");
    freeOrderedList(MyList);
    sleep(3);
    return 0;

}