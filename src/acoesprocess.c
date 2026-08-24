#include <stdio.h>
#define _POSIX_C_SOURCE 200809L   // necessário para o protótipo de strdup
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
            continue;  // não trava, segue pra próxima
        }
        pid_t pid = executar_task(t, -1, -1, diretorio_atual);
        if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Aviso: '%s' terminou com código %d\n",
                        t->nome, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                fprintf(stderr, "Aviso: '%s' foi morto pelo sinal %d\n",
                        t->nome, WTERMSIG(status));
            }
        }
    }
}
void cmd_run_parallel(char *nomes_tasks[], int n, TaskRegistry *reg) {
    ProcessoLancado lancados[MAX_PARALLEL];
    int total_lancados = 0;
 
   //lança TODOS os processos primeiro, sem esperar nenhum
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
            if (chdir(diretorio_atual) != 0) {
                fprintf(stderr, "Erro: diretório '%s' não encontrado\n", diretorio_atual);
                _exit(1);
            }
            aplicar_redirecionamentos(t);
            execvp(t->programa, t->argv);
            fprintf(stderr, "Erro: não foi possível executar '%s': %s\n",
                    t->programa, strerror(errno));
            _exit(127);
        }
 
        // processo pai: guarda o pid, NÃO espera ainda
        lancados[total_lancados].pid = pid;
        strncpy(lancados[total_lancados].nome_task, t->nome, 63);
        lancados[total_lancados].nome_task[63] = '\0';
        lancados[total_lancados].valido = 1;
        total_lancados++;
    }
 
    // só agora espera todos, um por um
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
void cmd_run_pipe(char *nomes[], int n, TaskRegistry *reg) {
    if (n < 2) {
        fprintf(stderr, "Erro: pipe requer pelo menos 2 tarefas\n");
        return;
    }
    if (n - 1 > MAX_PIPE) {
        fprintf(stderr, "Erro: pipe com muitas tarefas (máximo %d)\n", MAX_PIPE + 1);
        return;
    }
 
    int pipes[MAX_PIPE][2];
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            // fecha o que já foi aberto antes de desistir
            for (int j = 0; j < i; j++) { close(pipes[j][0]); close(pipes[j][1]); }
            return;
        }
    }
 
    pid_t pids[MAX_PIPE + 1];
 
    for (int i = 0; i < n; i++) {
        Task *t = buscar_task(reg, nomes[i]);
        if (t == NULL) {
            fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", nomes[i]);
            pids[i] = -1;
            continue;
        }
 
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            pids[i] = -1;
            continue;
        }
 
        if (pid == 0) {
            // ---- processo filho ----
            if (chdir(diretorio_atual) != 0) {
                fprintf(stderr, "Erro: diretório '%s' não encontrado\n", diretorio_atual);
                _exit(1);
            }
 
            // entrada: vem do pipe anterior, ou do input_file da própria task (se for a 1ª)
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            } else if (t->input_file != NULL) {
                int fd_in = open(t->input_file, O_RDONLY);
                if (fd_in < 0) {
                    fprintf(stderr, "Erro: não foi possível abrir '%s': %s\n",
                            t->input_file, strerror(errno));
                    _exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
 
            // saída: vai pro próximo pipe, ou pro output_file da própria task (se for a última)
            if (i < n - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            } else if (t->output_file != NULL) {
                int flags = O_WRONLY | O_CREAT | (t->append_mode ? O_APPEND : O_TRUNC);
                int fd_out = open(t->output_file, flags, 0644);
                if (fd_out < 0) {
                    fprintf(stderr, "Erro: não foi possível abrir '%s': %s\n",
                            t->output_file, strerror(errno));
                    _exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
 
            // fecha TODOS os fds de pipe no filho (já duplicados no que precisava)
            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
 
            execvp(t->programa, t->argv);
            fprintf(stderr, "Erro: não foi possível executar '%s': %s\n",
                    t->programa, strerror(errno));
            _exit(127);
        }
 
        pids[i] = pid;
    }
 
    // ---- processo pai: fecha todos os fds de pipe ----
    for (int i = 0; i < n - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
 
    // espera todo mundo
    for (int i = 0; i < n; i++) {
        if (pids[i] > 0) {
            int status;
            waitpid(pids[i], &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Aviso: '%s' terminou com código %d\n",
                        nomes[i], WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                fprintf(stderr, "Aviso: '%s' foi morto pelo sinal %d\n",
                        nomes[i], WTERMSIG(status));
            }
        }
    }
}
void cmd_input(TaskRegistry *reg, char *nome_task, char *arquivo) {
    Task *t = buscar_task(reg, nome_task);
    if (t == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", nome_task);
        return;
    }
    t->input_file = strdup(arquivo);
}
void cmd_output(TaskRegistry *reg, char *nome_task, char *arquivo) {
    Task *t = buscar_task(reg, nome_task);
    if (t == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", nome_task);
        return;
    }
    t->output_file = strdup(arquivo);
    t->append_mode = 0;
}
void cmd_append(TaskRegistry *reg, char *nome_task, char *arquivo) {
    Task *t = buscar_task(reg, nome_task);
    if (t == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", nome_task);
        return;
    }
    t->output_file = strdup(arquivo);
    t->append_mode = 1;
}
void cmd_workdir(char *dir) {
    struct stat st;
    if (stat(dir, &st) != 0 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Erro: diretório '%s' não encontrado\n", dir);
        return;
    }
    strncpy(diretorio_atual, dir, 255);
    diretorio_atual[255] = '\0';
}
void cmd_start(TaskRegistry *reg, JobRegistry *jreg, char *nome_task) {
    Task *t = buscar_task(reg, nome_task);
    if (t == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", nome_task);
        return;
    }
    pid_t pid = executar_task(t, -1, -1, diretorio_atual);
    if (pid <= 0) {
        return;  // executar_task já imprimiu o erro
    }
    int job_id = cadastrar_job(jreg, pid, nome_task);
    if (job_id > 0) {
        printf("[%d] %d\n", job_id, pid);
    }
}
int processar_comando(int argc, char *argv[], TaskRegistry *reg, JobRegistry *jreg) {
    if (argc == 0) {
        return 0;  // linha vazia, já filtrada antes de chegar aqui, mas por segurança
    }
 
    if (strcmp(argv[0], "exit") == 0) {
        return 1;
    }
 
    if (strcmp(argv[0], "task") == 0) {
        cadastrar_task(argv, argc, reg);
    }
    else if (strcmp(argv[0], "run") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Uso: run <sequential|parallel|pipe> <tarefas...>\n");
            return 0;
        }
        char **nomes = &argv[2];
        int n = argc - 2;
 
        if (strcmp(argv[1], "sequential") == 0) {
            cmd_run_sequential(nomes, n);
        } else if (strcmp(argv[1], "parallel") == 0) {
            cmd_run_parallel(nomes, n, reg);
        } else if (strcmp(argv[1], "pipe") == 0) {
            cmd_run_pipe(nomes, n, reg);
        } else {
            fprintf(stderr, "Erro: modo de execução '%s' desconhecido (use sequential, parallel ou pipe)\n", argv[1]);
        }
    }
    else if (strcmp(argv[0], "input") == 0) {
        if (argc != 3) { fprintf(stderr, "Uso: input <tarefa> <arquivo>\n"); return 0; }
        cmd_input(reg, argv[1], argv[2]);
    }
    else if (strcmp(argv[0], "output") == 0) {
        if (argc != 3) { fprintf(stderr, "Uso: output <tarefa> <arquivo>\n"); return 0; }
        cmd_output(reg, argv[1], argv[2]);
    }
    else if (strcmp(argv[0], "append") == 0) {
        if (argc != 3) { fprintf(stderr, "Uso: append <tarefa> <arquivo>\n"); return 0; }
        cmd_append(reg, argv[1], argv[2]);
    }
    else if (strcmp(argv[0], "workdir") == 0) {
        if (argc != 2) { fprintf(stderr, "Uso: workdir <diretorio>\n"); return 0; }
        cmd_workdir(argv[1]);
    }
    else if (strcmp(argv[0], "start") == 0) {
        if (argc != 2) { fprintf(stderr, "Uso: start <tarefa>\n"); return 0; }
        cmd_start(reg, jreg, argv[1]);
    }
    else if (strcmp(argv[0], "jobs") == 0) {
        listar_jobs(jreg);
    }
    else if (strcmp(argv[0], "wait") == 0) {
        if (argc != 2) { fprintf(stderr, "Uso: wait <jobId>\n"); return 0; }
        int job_id = atoi(argv[1]);
        esperar_job(jreg, job_id);
    }
    else {
        fprintf(stderr, "Erro: comando '%s' não reconhecido\n", argv[0]);
    }
 
    return 0;
}

