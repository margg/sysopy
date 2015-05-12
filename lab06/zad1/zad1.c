/*
 *
 * każda para klient serwer ma swoją kolejkę do komunikacji
 * (zeby nie bawić się w semafory na razie)
 *
 * serwer ma info o swoich kolejkach - ilu klientów jest uruchomionych
 *
 * klient wysyła do serwera -> format! żeby dało się odpowiedzieć
 *
 * klient może zakończyć przez "exit" i ctrl c i z
 *
 * obsłużyć ctrl C i Z -> zamknąć kolejkę!!
 *
 *
 * dla BSD i System V - są różne!
 *
 * shlctl
 *
 * ftok -> generuje klucz dla kolejki
 *
 * msg.... ->
 *  msgctl
 *  msgget
 *  msgrcv
 *
 *
 * IPC_... flagi !
 *
 *
 *
 * POSIX
 *
 * mqopen
 * mqclose
 * mqunlink
 * mqsend
 * mqreceive
 *
 *
 * Dla chętnych -> wykorzystać funkcję mqnotify
 *
 *
 *
 *
 */