//implementação das funções de task TERMINADA
#define MAX_TASKS 100
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "task.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
 // open, O_RDONLY, O_WRONLY, O_CREAT, O_APPEND, O_TRUNC
#include <fcntl.h>
void cadastrar_task(char *argv[], int argc, TaskRegistry *reg) {
    // argv[0] = "task", argv[1] = nome, argv[2] = programa, argv[3..] = args

    if (argc < 3) {
        fprintf(stderr, "Uso: task <nome> <programa> [argumentos...]\n");
        return;
    }

    if (reg->num_tasks >= MAX_TASKS) {
        fprintf(stderr, "Erro: você chegou ao limite no cadastro de tarefas\n");
        return;
    }

    Task *t = &reg->tasks[reg->num_tasks];

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

    reg->num_tasks++;
}


pid_t executar_task(Task *task, int fd_entrada, int fd_saida, char *workdir) {
    if (task == NULL) {
        fprintf(stderr, "Erro: tarefa inválida\n");
        return -1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // ---- PROCESSO FILHO ----

        if (workdir != NULL) {
            if (chdir(workdir) != 0) {
                fprintf(stderr, "Erro: diretório '%s' não encontrado\n", workdir);
                exit(1);
            }
        }

        // ENTRADA: prioridade pro fd explícito (ex: pipe); senão, usa input_file da task
        if (fd_entrada != -1) {
            dup2(fd_entrada, STDIN_FILENO);
            close(fd_entrada);
        } else if (task->input_file != NULL) {
            int fd_in = open(task->input_file, O_RDONLY);
            if (fd_in < 0) {
                fprintf(stderr, "Erro: não foi possível abrir '%s': %s\n",
                        task->input_file, strerror(errno));
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // SAÍDA: prioridade pro fd explícito (ex: pipe); senão, usa output_file da task
        if (fd_saida != -1) {
            dup2(fd_saida, STDOUT_FILENO);
            close(fd_saida);
        } else if (task->output_file != NULL) {
            int flags = O_WRONLY | O_CREAT | (task->append_mode ? O_APPEND : O_TRUNC);
            int fd_out = open(task->output_file, flags, 0644);
            if (fd_out < 0) {
                fprintf(stderr, "Erro: não foi possível abrir '%s': %s\n",
                        task->output_file, strerror(errno));
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        execvp(task->programa, task->argv);

        fprintf(stderr, "Erro: não foi possível executar '%s': %s\n",
                task->programa, strerror(errno));
        exit(127);
    }

    // ---- PROCESSO PAI ----
    if (fd_entrada != -1) close(fd_entrada);
    if (fd_saida != -1) close(fd_saida);

    return pid;
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
        return &pTask->tasks[indice];
    }
    else{
        return NULL;
    }
}
