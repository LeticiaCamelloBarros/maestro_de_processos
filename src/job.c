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

void atualizar_status_job(JobRegistry *registry, pid_t pid, int status){

}

void coletar_todos_jobs(JobRegistry *registry){

}

