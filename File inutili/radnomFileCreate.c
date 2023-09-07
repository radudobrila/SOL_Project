#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // Per la funzione sleep

void create_binary_file(const char *filename) {
    FILE *file = fopen(filename, "wb");
    
    if (file == NULL) {
        printf("Errore: Impossibile creare il file '%s'\n", filename);
        return;
    }

    for (int i = 0; i < 5; i++) {
        int random_number = rand() % 10 + 1; // Genera un numero casuale da 1 a 100
        fwrite(&random_number, sizeof(int), 1, file);
    }

    fclose(file);
}

int main() {
    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    int n = 10; // Numero di file da creare

    for (int i = 1; i <= n; i++) {
        char filename[20];
        snprintf(filename, sizeof(filename), "prova_%d.dat", i);
        create_binary_file(filename);

        // Aggiungi un ritardo di 1 secondo tra le chiamate
        sleep(1);
    }

    printf("%d file creati con successo.\n", n);

    return 0;
}
