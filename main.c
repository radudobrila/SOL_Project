// Progetto di Sistemi Operativi

int main(int argc, const char *argv[]){
    int n, q;
    if(argc < 1){
        perror("Error: Invalid_Argument\n");
        return -1;
    }

    for( int i = 0; i < 4; i++){
        if(strcmp(argv[i], "-n") == 1) n = argv[i+1];
        if(strcmp(argv[i], "-q") == 1) q = argv[i+1];
    }

    
}