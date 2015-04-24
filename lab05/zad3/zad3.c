#include <stdio.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <fcntl.h>

/*
 *
 *
 *
 *
 *
 * łącza nazwane -> fifo
 *

 *
 *
 * -> odczyt wiąże się z wyczyszczeniem zawartości
 * -> fifo -> dostępna jako plik na dysku, można ją podejrzeć
 *
 * program musi znać nazwe jaką nadajemy kolejce
 *
 *
 * łącza
 * - przy otwarciu - flagi, ustawienia, (no_block, no_delay)
 * -
 *
 * łącza nienazwane
 * -> dostęo sekwencyjny
 * -> odczyt usuwa zawartość
 * -> obsługa - niskopoziomowa, wysokopoziomowa
 *
 * zad1 - niskopoziomowo
 * -> pipe - pobiera tablicę deskryptorów (0, 1, 2 - stderr)
 * -> dup(), dup2() -> duplikacja deskryptorów
 * ->
 *
 *
 * zad2 - wysokopoziomowo
 * -> operujemy na strumieniach plikowych
 * -> możemy założyć, że (ls |grep ) może być całe w execu(tylko w tym zadaniu!), później przekierowanie
 *
 *
 */


int main(int argc, char **argv) {

    FILE *fp;
    char path[PATH_MAX];

    fp = popen("ls -l | grep ^d", "r");

    FILE *file = fopen("folders.txt", "w");

    if (fp == NULL) {
        printf("Error while opening the pipe.\n\n");
        exit(-1);
    }

    while (fgets(path, PATH_MAX, fp) != NULL) {
        fputs(path, file);
        printf("%s", path);
    }

    pclose(fp);
    fclose(file);

    return 0;
}