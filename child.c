#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if (argc != 1){
        fprintf(stderr, "Необходимое количество аргументов 1: название процесса");
        return 1;
    }
    ssize_t len;
    size_t cap = 0;
    char *word = NULL;

    while((len = getline(&word, &cap, stdin)) != -1){
        size_t j = 0;
        for (ssize_t i = 0; i<len; i++){
            if (strchr("aeiouyAEIOUY", word[i]) == NULL){
                word[j] = word[i];
                j++;
            }
        }
        word[j] = '\0';
        if (printf("%s", word) == -1){
            perror("printf");
            free(word);
            return 1;
        }
        if (fflush(stdout) == -1){
            perror("fflush");
            free(word);
            return 1;
        }
        fprintf(stderr, "(%s pid=%d) записал слово в файл: %s", argv[0], getpid(), word);
    }
    free(word);
    return 0;
}
