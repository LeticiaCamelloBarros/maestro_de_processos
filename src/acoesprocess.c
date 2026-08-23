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
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void aplicar_redirecionamentos(Task *t) {
    // --- ENTRADA (input) ---
    if (t->input_file != NULL) {
        int fd_in = open(t->input_file, O_RDONLY);
        if (fd_in < 0) {
            fprintf(stderr, "Erro: não foi possível abrir arquivo de entrada '%s': %s\n",
                    t->input_file, strerror(errno));
            _exit(1);
        }
        if (dup2(fd_in, STDIN_FILENO) < 0) {
            perror("dup2 (input)");
            close(fd_in);
            _exit(1);
        }
        close(fd_in);  // já foi duplicado, o original pode fechar
    }

    // --- SAÍDA (output ou append) ---
    if (t->output_file != NULL) {
        int flags = O_WRONLY | O_CREAT;
        flags |= t->append_mode ? O_APPEND : O_TRUNC;

        int fd_out = open(t->output_file, flags, 0644);
        if (fd_out < 0) {
            fprintf(stderr, "Erro: não foi possível abrir arquivo de saída '%s': %s\n",
                    t->output_file, strerror(errno));
            _exit(1);
        }
        if (dup2(fd_out, STDOUT_FILENO) < 0) {
            perror("dup2 (output)");
            close(fd_out);
            _exit(1);
        }
        close(fd_out);
    }
}
int tokenizar(char *linha, char *argv[]) {
    int argc = 0;
    char *token = strtok(linha, DELIM);

    while (token != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, DELIM);
    }

    argv[argc] = NULL;  // execvp e afins esperam terminação em NULL
    return argc;         // ESSENCIAL: sem isso, quem chama recebe lixo
}
void processar_linha(char *linha){

}
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

void cmd_run_parallel(char *nomes_tasks[], int n, TaskRegistry *reg) {

    ProcessoLancado lancados[MAX_PARALLEL];
    int total_lancados = 0;

    // FASE 1: lança TODOS os processos primeiro, sem esperar nenhum
    for (int i = 0; i < n; i++) {
        Task *t = buscar_task(reg, nomes_tasks[i]);

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