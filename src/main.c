#define MAX_ARGS 64
#define DELIM " \t\r\n"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//para mexer com essa parte do so 
#include <sys/types.h>
#include <unistd.h>
//importando os arquivos de bibliotecas locais : 
#include <task.h> 
#include <job.h> 
#define qntd_max 100
int tokenizar(char *linha, char *argv[]) {
    int capacidade = 8;
    char **argv = malloc(sizeof(*argv) * sizeof(char *));
    int argc = 0;
    char *token = strtok(linha, DELIM);
    while (token != NULL) {
        if (argc >= capacidade - 1) {
            capacidade *= 2;
            argv = realloc(argv, capacidade * sizeof(char *));
        }
        argv[argc++] = token;
        token = strtok(NULL, DELIM);
}
argv[argc] = NULL;
}

int main(int argc, char *argv[]) {
    char linha[1024];
    char *meu_argv[MAX_ARGS];
    int meu_argc;
    if (argc == 1) {
        // modo interativo (nenhum argumento extra além do nome do programa)
    } else {
        // modo workflow
    }
    while (true) {
        printf("meuterminal");
        
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            break;  // EOF (ctrl+D)
        }

        meu_argc = tokenizar(linha, meu_argv);

        if (meu_argc == 0) {
            continue;  // linha vazia ou só espaços -> ignora
        }

        // aqui meu_argv[0] é o comando, meu_argv[1..] são os args
        executar(meu_argc, meu_argv);
    }
    free(meu_argv);  // - libera a CADA iteração, antes de ler a próxima linha
    return 0;
}
