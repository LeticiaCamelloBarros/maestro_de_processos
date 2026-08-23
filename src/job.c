#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "job.h"
#include "task.h"
#define MAX_JOBS 100
// Cria um novo job na lista, devolve o job_id gerado
int cadastrar_job(JobRegistry *registry, pid_t pid, const char *nome_task) {
    if (registry->num_jobs >= MAX_JOBS) {
        fprintf(stderr, "Erro: limite de jobs atingido\n");
        return -1;  // caminho de erro: retorna -1
    }

    Job *j = &registry->jobs[registry->num_jobs];
    j->job_id = registry->next_job_id++;
    j->PID = pid;
    strncpy(j->nome_task, nome_task, sizeof(j->nome_task) - 1);
    j->nome_task[sizeof(j->nome_task) - 1] = '\0';
    j->status = JOB_RUNNING;
    registry->num_jobs++;

    return j->job_id;  // caminho de sucesso: retorna o id criado — ESSENCIAL, não pode faltar
}

Job* buscar_job(JobRegistry *registry, int job_id){
    bool found = false;
    int indice ;
for(int i =0;i<(registry->num_jobs);i++){
//    Job jobs[MAX_JOBS];

if((registry->jobs[i].job_id)==job_id) {
  found = true ;
  indice = i ; 
  break;
}
}
if (found==false){
    printf("job não encontrado ");
    return NULL;
}else {
    printf("job encontrado ");
    return &registry->jobs[indice];
}
}



void listar_jobs(JobRegistry *registry) {
    for (int i = 0; i < registry->num_jobs; i++) {
        Job *j = &registry->jobs[i];
        printf("[%d]  ", j->job_id);
        switch (j->state) {
            case JOB_RUNNING:
                printf("Running    %s\n", j->nome_task);
                break;
            case JOB_DONE:
                printf("Done       %s (exit %d)\n", j->nome_task, WEXITSTATUS(j->status));
                break;
            case JOB_SIGNALED:
                printf("Signaled   %s (signal %d)\n", j->nome_task, WTERMSIG(j->status));
                break;
        }
    }
}

void atualizar_status_job(JobRegistry *registry, pid_t pid, int status) {
    // 1. encontra o job correspondente a esse PID
    Job *j = NULL;
    for (int i = 0; i < registry->num_jobs; i++) {
        if (registry->jobs[i].PID == pid) {
            j = &registry->jobs[i];
            break;
        }
    }

    // 2. se não achou (situação anômala, mas defensiva), não faz nada
    if (j == NULL) {
        return;
    }

    // 3. guarda o valor cru, sempre — útil pra debug e pra WEXITSTATUS depois
    j->status = status;

    //se protege contra essa rúbrica : "Processos que terminam com código de saída diferente de zero" (situação a tratar de forma coerente)
    if (WIFEXITED(status)) {
        j->state = JOB_DONE;
    } else if (WIFSIGNALED(status)) {
        j->state = JOB_SIGNALED;
    } else {
        // WIFSTOPPED ou outro caso raro (processo pausado, não terminado de verdade)
        // não muda running nem state, porque o job ainda não acabou de fato
        return;
    }
}

void coletar_todos_jobs(JobRegistry *registry){

}

