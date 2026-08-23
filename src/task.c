#define MAX_TASKS 100
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "task.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
void cadastrar_task(char *argv[], int argc, TaskRegistry *registry) {
    // argv[0] = "task", argv[1] = nome, argv[2] = programa, argv[3..] = args

    if (argc < 3) {
        fprintf(stderr, "Uso: task <nome> <programa> [argumentos...]\n");
        return;
    }

    if (registry->num_tasks >= MAX_TASKS) {
        fprintf(stderr, "Erro: você chegou ao limite no cadastro de tarefas\n");
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


pid_t executar_task(Task *task, int fd_entrada, int fd_saida, char *workdir) {
    if (task == NULL) {
        fprintf(stderr, "Erro: tarefa inválida\n");
        return -1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        //se o pid for menor q 0 houve um erro na criação do processo filho 
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // ---- PROCESSO FILHO ----

        // 1. muda diretório de trabalho, se especificado
        if (workdir != NULL) {
            if (chdir(workdir) != 0) {
                fprintf(stderr, "Erro: diretório '%s' não encontrado\n", workdir);
                exit(1);
            }
        }

        // 2. redireciona entrada, se necessário
        if (fd_entrada != -1) {
            if (dup2(fd_entrada, STDIN_FILENO) < 0) {
                perror("dup2 entrada");
                exit(1);
            }
            close(fd_entrada);
        }

        // 3. redireciona saída, se necessário
        if (fd_saida != -1) {
            if (dup2(fd_saida, STDOUT_FILENO) < 0) {
                perror("dup2 saida");
                exit(1);
            }
            close(fd_saida);
        }

        // 4. executa o programa da tarefa
        execvp(task->programa, task->argv);

        // só chega aqui se execvp falhar
        fprintf(stderr, "Erro: não foi possível executar '%s': %s\n",
                task->programa, strerror(errno));
        exit(127);  // convenção: 127 = comando não encontrado
    }

    // ---- PROCESSO PAI ----
    // fecha os fds que foram passados (o pai não precisa mais deles,
    // já que o filho tem sua própria cópia via dup2)
    if (fd_entrada != -1) close(fd_entrada);
    if (fd_saida != -1) close(fd_saida);

    return pid;  // devolve o PID pra quem chamou decidir o que fazer
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
