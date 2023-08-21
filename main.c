// Progetto di Sistemi Operativi
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, const char *argv[]){
    int n, q;
    if(argc < 1){
        perror("Error: Invalid_Argument\n");
        return -1;
    }

    for( int i = 1; i < 5; i++){
        printf("i: %d, argv: %s\n", i, argv[i]);
        if(strcmp(argv[i], "-n") == 0){
            n = atoi(argv[i+1]);
        }
        if(strcmp(argv[i], "-q") == 0){ 
            q = atoi(argv[i+1]);
        }
    }

    
    
}