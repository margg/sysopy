#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WRITE_END 1
#define READ_END 0


void spawnLs(int lsToGrep[], int grepToWc[]) ;

void spawnGrep(int lsToGrep[], int grepToWc[]) ;

void performWc(int lsToGrep[], int grepToWc[]) ;

void execPipedWc(int grepToWc[]) ;

void execPipedGrep(int lsToGrep[], int grepToWc[]) ;

void execPipedLs(int lsToGrep[]) ;

int main(int argc, char **argv) {

    int lsToGrep[2];
    int grepToWc[2];

    pipe(lsToGrep);
    pipe(grepToWc);

    spawnLs(lsToGrep, grepToWc);
    spawnGrep(lsToGrep, grepToWc);
    performWc(lsToGrep, grepToWc);

    return 0;
}


void closeUnusedPipe(int pipe[]) {
    close(pipe[WRITE_END]);
    close(pipe[READ_END]);
}

void spawnLs(int lsToGrep[], int grepToWc[]) {
    pid_t pid = fork();
    if (pid == 0) {
        closeUnusedPipe(grepToWc);
        execPipedLs(lsToGrep);
    }
}

void spawnGrep(int lsToGrep[], int grepToWc[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execPipedGrep(lsToGrep, grepToWc);
    }
}

void performWc(int lsToGrep[], int grepToWc[]) {
    closeUnusedPipe(lsToGrep);
    execPipedWc(grepToWc);
}

void execPipedLs(int lsToGrep[]) {
    close(lsToGrep[READ_END]);
    dup2(lsToGrep[WRITE_END], STDOUT_FILENO);
    close(lsToGrep[WRITE_END]);
    execlp("ls", "ls", "-l", NULL);
}

void execPipedGrep(int lsToGrep[], int grepToWc[]) {
    close(lsToGrep[WRITE_END]);
    dup2(lsToGrep[READ_END], STDIN_FILENO);
    close(lsToGrep[READ_END]);
    close(grepToWc[READ_END]);
    dup2(grepToWc[WRITE_END], STDOUT_FILENO);
    close(grepToWc[WRITE_END]);
    execlp("grep", "grep", "^g", NULL);
}

void execPipedWc(int grepToWc[]) {
    close(grepToWc[WRITE_END]);
    dup2(grepToWc[READ_END], STDIN_FILENO);
    close(grepToWc[READ_END]);
    execlp("wc", "wc", "-l", NULL);
}




