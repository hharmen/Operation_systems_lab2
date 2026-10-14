#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <string.h>


int main(void){
//=====Объявил переменные==========
    signal(SIGPIPE, SIG_IGN);
    srand(time(NULL));
    char *word = NULL;
    char *name1 = NULL;
    char *name2 = NULL;
    int res = 0;
    int file1 = -1;
    int file2 = -1;
    int pid1 = -1;
    int pid2 = -1;
    int pipefd1[2] = {-1, -1};
    int pipefd2[2] = {-1, -1};
    if (pipe(pipefd1) == -1){
        perror("pipe");
        res = 1; 
        goto end;
    }
    if (pipe(pipefd2) == -1){
        perror("pipe");
        res = 1; 
        goto end;
    }
    
//=======Считываю ввод имен файлов========    
    
    printf("(parent pid=%d) Введите название первого файла: ", getpid());
    size_t cap1 = 0;
    ssize_t len1 = getline(&name1, &cap1, stdin);
    if (len1 == -1){
        fprintf(stderr, "(parent pid=%d) Не удалось считать название файла\n", getpid());
        res = 1; 
        goto end;
    }
    if (name1[len1-1] == '\n'){
        name1[len1-1] = '\0';
        len1--;
    }
    if (len1 <= 0){
        fprintf(stderr, "(parent pid=%d) Нельзя вводить пустое название файла\n", getpid());
        res = 1; 
        goto end;
    }
    
    printf("(parent pid=%d) Введите название второго файла: ", getpid());
    size_t cap2 = 0;
    ssize_t len2 = getline(&name2, &cap2, stdin);
    if (len2 == -1){
        fprintf(stderr, "(parent pid=%d) Не удалось считать название файла\n", getpid());
        res = 1; 
        goto end;
    }
    if (name2[len2-1] == '\n'){
        name2[len2-1] = '\0';
        len2--;
    }
    if (len2 <= 0){
        fprintf(stderr, "(parent pid=%d) Нельзя вводить пустое название файла\n", getpid());
        res = 1; 
        goto end;
    }

    if (strcmp(name1, name2) == 0) {
        fprintf(stderr,"(parent pid=%d) Нельзя вводить одинаковые названия\n", getpid());
        res = 1;
        goto end;
    }

//======открываю первый файл и запускаю потомка первого=======

    file1 = open(name1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file1 == -1) {
        perror("open");
        res = 1; 
        goto end;
    }

    fflush(stdout);
    pid1 = fork();
    if(pid1 == -1){
        perror("fork");
        res = 1; 
        goto end;
    }
    if (pid1 == 0) {
        if (dup2(pipefd1[0], 0) == -1){
            perror("dup2");
            exit(1);
        }
        if (dup2(file1, 1) == -1){
            perror("dup2");
            exit(1);
        }
        close(pipefd1[1]);
        close(pipefd1[0]);
        close(pipefd2[1]);
        close(pipefd2[0]);
        close(file1);
        close(file2);
        free(name2);
        free(name1);
        execl("./child", "child1", NULL);
        perror("execl");
        exit(1);
    }
    printf("(parent pid=%d) создал процесс child1\n", getpid());
    close(file1);
    file1 = -1;
    close(pipefd1[0]);
    pipefd1[0] = -1;

//======открываю второй файл и запускаю потомка второго=======

    file2 = open(name2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file2 == -1) {
        perror("open");
        res = 1; 
        goto end;
    }

    fflush(stdout);
    pid2 = fork();
    if(pid2 == -1){
        perror("fork");
        res = 1; 
        goto end;
    }
    if (pid2 == 0) {
        if (dup2(pipefd2[0], 0) == -1){
            perror("dup2");
            exit(1);
        }
        if (dup2(file2, 1) == -1){
            perror("dup2");
            exit(1);
        }
        close(pipefd2[1]);
        close(pipefd2[0]);
        close(pipefd1[1]);
        close(pipefd1[0]);
        close(file1);
        close(file2);
        free(name2);
        free(name1);
        execl("./child", "child2", NULL);
        perror("execl");
        exit(1);
    }
    printf("(parent pid=%d) создал процесс child2\n", getpid());
    close(file2);
    file2 = -1;
    close(pipefd2[0]);
    pipefd2[0] = -1;

//========Начинаем принимать на вход слова========

    size_t cap = 0;
    ssize_t len;
    printf("(parent pid=%d) Введите слово: ", getpid());
    while ((len = getline(&word, &cap, stdin)) != -1){
        if (rand()%100 < 80){
            printf("(parent pid=%d) отправил слово в процесс child1\n", getpid());
            if (write(pipefd1[1], word, len) == -1){
                perror("write");
                res = 1; 
                goto end;
            }
        }else {
            printf("(parent pid=%d) отправил слово в процесс child2\n", getpid());
            if (write(pipefd2[1], word, len) == -1){
                perror("write");
                res = 1; 
                goto end;
            }
        }
        printf("(parent pid=%d) Введите слово: ", getpid());
    }

end:
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[1]);
    close(pipefd2[0]);
    close(file1);
    close(file2);
    free(word);
    free(name1);
    free(name2);
    if (pid1 > 0) waitpid(pid1, NULL, 0);
    if (pid2 > 0) waitpid(pid2, NULL, 0);
    return res;

}