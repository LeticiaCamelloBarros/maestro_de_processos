#define MAX_TASKS 100
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <task.h>
void cadastro_de_tarefas (char  **argv[],int argc, TaskRegistry  *registry){
            // argv[0] = "task", argv[1] = nome, argv[2] = programa, argv[3..] = args

    if (argc < 3) {
        fprintf(stderr, "Uso: task <nome> <programa> [argumentos...]\n");
        return;
    }
    if (registry->num_tasks >= MAX_TASKS) {
        fprintf(stderr, "Erro: limite de tarefas cadastradas atingido\n");
        return;
    }

    Task *t = &registry->tasks[registry->num_tasks];
    strncpy(t->nome, argv[1], sizeof(t->nome) - 1);
    strncpy(t->programa, argv[2], sizeof(t->programa) - 1);

    // monta o argv que vai pro execvp: argv[2], argv[3], ..., NULL
    int i;
    for (i = 2; i < argc; i++) {
        t->argv[i - 2] = strdup(argv[i]);
    }
    t->argv[i - 2] = NULL;

    t->input_file = NULL;
    t->output_file = NULL;
    t->append_mode = 0;

    registry->num_tasks++;
}