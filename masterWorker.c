#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main(int argc, const char *argv[]){
    char mw_input[256];
    fgets(mw_input, sizeof(mw_input), stdin);

    int i = 0;
    while(strcmp(mw_input[i], NULL) == 0 ){
        printf("Arg[i] = %c", mw_input[i]);
    }

    return 0;
}