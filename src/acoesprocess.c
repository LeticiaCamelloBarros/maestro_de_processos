#include <stdio.h>
#include <stdlib.h>
//biblioteca para algumas manipulações de arquivos
#include <fcntl.h> 
//biblioteca para perror e erno 
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "task.h"
#include "acoesprocess.h"
#define MAX_PARALLEL 32

void cmd_run_sequential(char *nomes[], int n) {
    for (int i = 0; i < n; i++) {
        Task *t = buscar_task(&registry, nomes[i]);
        if (t == NULL) {
            fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", nomes[i]);
            continue;  // não trava, segue pra próxima — exatamente o que a rubrica pede
        }
        pid_t pid= executar_task(t, -1, -1, diretorio_atual);
        if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void run_parallel(char *nomes_tasks[], int n, TaskRegistry *registry) {
    ProcessoLancado lancados[MAX_PARALLEL];
    int total_lancados = 0;

    // FASE 1: lança TODOS os processos primeiro, sem esperar nenhum
    for (int i = 0; i < n; i++) {
        Task *t = buscar_task(registry, nomes_tasks[i]);

        if (t == NULL) {
            fprintf(stderr, "Erro: tarefa '%s' não existe\n", nomes_tasks[i]);
            continue;  // pula essa, mas NÃO aborta as outras
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            // processo filho
            aplicar_redirecionamentos(t);   // dup2 de input/output/append, se houver
            execvp(t->programa, t->argv);
            // só chega aqui se execvp falhou
            fprintf(stderr, "Erro: não foi possível executar '%s'\n", t->programa);
            _exit(127);
        }

        // processo pai: guarda o pid, NÃO espera ainda
        lancados[total_lancados].pid = pid;
        strncpy(lancados[total_lancados].nome_task, t->nome, 63);
        lancados[total_lancados].valido = 1;
        total_lancados++;
    }

    // FASE 2: só agora espera todos, um por um
    for (int i = 0; i < total_lancados; i++) {
        int status;
        waitpid(lancados[i].pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Aviso: '%s' terminou com código %d\n",
                    lancados[i].nome_task, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Aviso: '%s' foi morto pelo sinal %d\n",
                    lancados[i].nome_task, WTERMSIG(status));
        }
    }
}