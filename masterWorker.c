#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<getopt.h>

#define ext ".dat"

// funzione che prende un
int arg_int(const char *n){
    int tmp;
    tmp = atoi(n);
    return tmp;
}

int arg_d(const char *n){
    int tmp;
    tmp = atoi(n);
    return tmp;
}



int main(int argc, const char *argv[]){
    if(argc <= 1){
        perror("Error: Invalid_Argument\n");
        return -1;
    }

    int opt;
    
    while((opt = getopt(argc, argv, "n:q:d:t:")) != -1){
        switch (opt)
        {
        case 'n': 
           printf("n = %d\n",arg_int(optarg));
        break;
        case 'q':
            printf("q = %d\n",arg_int(optarg));
        break;
        case 'd':
            printf("d = %d\n",arg_d(optarg));
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

    for (int i = optind; i < argc; i++) {
        const char *filename = argv[i];
        size_t len = strlen(filename);
        size_t ext_len = strlen(ext);

        if (len > ext_len && strcmp(filename + len - ext_len, ext) == 0) {
            printf("Nome del file: %s\n", argv[i]);
        }
    }

    return 0;
}