#define _POSIX_C_SOURCE 200809L
#define MAX_ARGS 64
#define DELIM " \t\r\n"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "job.h"
#include "acoesprocess.h"

// variáveis globais — só DECLARAÇÕES aqui, sem instruções soltas
TaskRegistry registry = {0};
JobRegistry jobs = { .num_jobs = 0, .next_job_id = 1 };  // já inicializa next_job_id = 1
char diretorio_atual[256] = ".";

// tokeniza a linha usando um array FIXO, sem malloc/realloc

int main(int argc, char *argv[]) {
    char linha[1024];
    char *meu_argv[MAX_ARGS];
    int meu_argc;
    int sair = 0;

    // Importante: quando a saída não vai pra um terminal (ex: pipe/redirecionamento
    // usado por scripts de correção), o stdio usa buffer cheio por padrão, o que
    // pode fazer a saída do ProcessFlow aparecer DEPOIS da saída dos filhos, fora
    // de ordem. Forçamos buffer por linha pra manter a ordem determinística.
    setvbuf(stdout, NULL, _IOLBF, 0);

    if (argc == 1) {
        // modo interativo
        while (!sair) {
            printf("processflow> ");
            fflush(stdout);  // garante que o prompt apareça antes do fgets bloquear

            if (fgets(linha, sizeof(linha), stdin) == NULL) {
                printf("\n");
                break;  // EOF (ctrl+D) -> encerra como se fosse exit
            }

            meu_argc = tokenizar(linha, meu_argv);

            if (meu_argc == 0) {
                continue;  // linha vazia ou só espaços -> ignora
            }

            sair = processar_comando(meu_argc, meu_argv, &registry, &jobs);
        }
    } else if (argc == 2) {
        // modo workflow
        FILE *arquivo = fopen(argv[1], "r");
        if (arquivo == NULL) {
            fprintf(stderr, "Erro: não foi possível abrir o arquivo '%s'\n", argv[1]);
            return 1;
        }

        while (!sair && fgets(linha, sizeof(linha), arquivo) != NULL) {
            printf("%s", linha);  // enunciado exige imprimir a linha antes de processar
            if (linha[strlen(linha) - 1] != '\n') {
                printf("\n");  // garante quebra de linha se o arquivo não terminar com \n
            }

            char linha_copia[1024];
            strncpy(linha_copia, linha, sizeof(linha_copia) - 1);
            linha_copia[sizeof(linha_copia) - 1] = '\0';

            meu_argc = tokenizar(linha_copia, meu_argv);

            if (meu_argc == 0) {
                continue;
            }

            sair = processar_comando(meu_argc, meu_argv, &registry, &jobs);
        }

        if (!sair) {
            fprintf(stderr, "Aviso: arquivo de workflow terminou sem o comando 'exit'\n");
        }

        fclose(arquivo);
    } else {
        fprintf(stderr, "Uso: %s [workflowFile]\n", argv[0]);
        return 1;
    }

    // encerramento: garante que nenhum processo em background fique zumbi
    coletar_todos_jobs(&jobs);
    printf("Encerrando ProcessFlow...\n");

    return 0;
}