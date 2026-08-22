#include <stdlib.h>
#include <stdio.h>
#define MAX_TASKS 100
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

void cadastro_de_tarefas (char  *argv[],int argc, TaskRegistry  *registry);
