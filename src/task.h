#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_TASKS 100
extern TaskRegistry registry;
extern char diretorio_atual[256];

// Struct 1: representa UMA tarefa cadastrada
typedef struct {
    char  nome[64];
    char  programa[256];
    char *argv[32];
    char *input_file;

    char *output_file;
    int   append_mode;
} Task;

// Struct 2: representa TODAS as tarefas cadastradas até agora
typedef struct {
    Task tasks[MAX_TASKS];   // <-- esse campo aqui
    int  num_tasks;
} TaskRegistry;

void cadastrar_task (char  *argv[],int argc, TaskRegistry  *registry)

pid_t executar_task(Task *t, int fd_entrada, int fd_saida);

Task *buscar_task(TaskRegistry *pTask,char *nome);


