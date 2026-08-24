#ifndef ACOESPROCESS_H
#define ACOESPROCESS_H
//ifndef tem que ficar no topo //para evitar que inclua o conteúdo do header 2x 

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "task.h"
#include <string.h>
#include "job.h"
//movendo para cá já que é usada em main.c e em acoesprocess.c e assim evitando duplicação de loop
#define MAX_ARGS 64
#define DELIM " \t\r\n"
typedef struct processoL{
    pid_t pid;
    char  nome_task[64];
    int   valido;   // 0 se essa entrada nem chegou a rodar (ex: task não existe)
} ProcessoLancado;


// tokenização / parsing
int  tokenizar(char *linha, char *argv[]);

// execução de tarefas
void aplicar_redirecionamentos(Task *t);
void cmd_run_sequential(char *nomes[], int n);
void cmd_run_parallel(char *nomes_tasks[], int n, TaskRegistry *reg);
void cmd_run_pipe(char *nomes[], int n, TaskRegistry *reg);

// redirecionamento e workdir
void cmd_input(TaskRegistry *reg, char *nome_task, char *arquivo);
void cmd_output(TaskRegistry *reg, char *nome_task, char *arquivo);
void cmd_append(TaskRegistry *reg, char *nome_task, char *arquivo);
void cmd_workdir(char *dir);

// background / jobs
void cmd_start(TaskRegistry *reg, JobRegistry *jreg, char *nome_task);

// Parser/dispatcher que identifica o comando digitado e chama a função certa
// retorna 1 se o comando foi 'exit' (sinaliza pro main.c encerrar o loop), 0 caso contrário
int processar_comando(int argc, char *argv[], TaskRegistry *reg, JobRegistry *jreg);

#endif
