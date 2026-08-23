#include <stdio.h>
#include <stdlib.h>
#include "task.h"
//biblioteca para algumas manipulações de arquivos
#include <fcntl.h> 
//biblioteca para perror e erno 
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
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