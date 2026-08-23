#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "task.h"
#include "acoesprocess.h"
#include <string.h>
//para evitar que inclua o conteúdo do header 2x 
#ifndef ACOESPROCESS_H
#define ACOESPROCESS_H

typedef struct {
    pid_t pid;
    char  nome_task[64];
    int   valido;   // 0 se essa entrada nem chegou a rodar (ex: task não existe)
} ProcessoLancado;

void cmd_run_sequential(char *nomes[], int n) ;

void run_parallel(char *nomes_tasks[], int n, TaskRegistry *registry) ;

#endif
