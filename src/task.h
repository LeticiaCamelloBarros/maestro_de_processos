#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#define MAX_TASKS 100 
extern char diretorio_atual[256];
//para evitar que inclua o conteúdo do header 2x 
#ifndef TASK_H
#define TASK_H
// Struct 1: representa UMA tarefa cadastrada
typedef struct Task{
    char  nome[64];
    char  programa[256];
    char *argv[32];
    char *input_file;

    char *output_file;
    int   append_mode;
} Task;

// Struct 2: representa TODAS as tarefas cadastradas até agora
typedef struct TaskRegistry{
    Task tasks[MAX_TASKS];   // <-- esse campo aqui
    int  num_tasks;
} TaskRegistry;
extern TaskRegistry registry;

void cadastrar_task (char  *argv[],int argc, TaskRegistry  *reg);

pid_t executar_task(Task *task, int fd_entrada, int fd_saida, char *workdir) ;

Task *buscar_task(TaskRegistry *pTask,char *nome);


#endif