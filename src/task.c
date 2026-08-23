#define MAX_TASKS 100
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "task.h"
#include <stdbool.h>
#include <string.h>
void cadastrar_task(char *argv[], int argc, TaskRegistry *registry) {
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
    t->nome[sizeof(t->nome) - 1] = '\0';

    strncpy(t->programa, argv[2], sizeof(t->programa) - 1);
    t->programa[sizeof(t->programa) - 1] = '\0';

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


pid_t executar_task(Task *t, int fd_entrada, int fd_saida){

}

Task *buscar_task(TaskRegistry *pTask,char  *nome){
    bool found = false ;
    int indice = 0 ; 
    for(int i=0;i<pTask->num_tasks;i++){
        if (strcmp(pTask->tasks[i].nome, nome) == 0){
            found = true ; 
            indice = i ; 
            break ;//pode parar aqui já achou//
        }
    }
    if (found){
        printf("task não  encontrada");
        return &pTask->tasks[indice];
    }
    else{
        printf("task não encontrada");
        return NULL;
    }
}
