/*
*
* sygnały których nie można obsłużyć
*
*
* SIGINT, SIGTERM
*
* napisać program w którym obsłużymy te sygnały za pomocą signal() i sigaction()
*
* program wypisuje cyferki 1,2,3,4....10, 1,2,3...
*
* SIGINT -> malejąco - 9,8,7,6...
* SIGTERM -> koniec programu
*
*
*
*
* zad1
* -> wysyłanie sygnałów - kill(), raise() do siebie,
* -> definiowanie własnych - sigset, sigunset,
* -> sigprocmas()
* -> sigkill() sigterm()
*
* sygnały mogą nyć przechwycone albo zignorowane
* -> sygnały niezawodne - muszą być obsłużone
*
*
* nie kombinować, nie robić opóźnień - to, że one się pogubią z drugiej strony, to normalka, tak powinno być
*
* przesłać numer procesu jako argument
*
*
*
* zad2
* inaczej:
* -> sygnały czasu rzecz. - nie giną, wymagają potwierdzenia, wszystkie muszą być przechwycone
*
* parent czeka na sygnał od potomka żeby zacząć nadawanie
*
*
*
*/

#include<stdio.h>
#include<signal.h>
#include <unistd.h>
#include <stdlib.h>


int counter = 0;

int diffenerce = 1;

void sig_handler(int signal) {
    if(signal == SIGINT) {
        printf("Received SIGINT (signal no. %d) - reversing\n", signal);
        diffenerce = -diffenerce;
    } else if (signal == SIGTERM){
        printf("Received SIGTERM (signal no. %d)\n\nBye!\n\n", signal);
        exit(1);
    }
}

int main(void) {
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        printf("\ncan't catch SIGINT\n");
    }
    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        printf("\ncan't catch SIGTERM\n");
    }

    while (1) {
//        for(counter = 1; counter<=10;) {
        printf("%d\n", counter);
        counter = counter + diffenerce;
        sleep(1);
//        }
    };

    return 0;
}