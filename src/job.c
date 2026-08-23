#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "job.h"
#include "task.h"
#define MAX_JOBS 100
// Cria um novo job na lista, devolve o job_id gerado
int cadastrar_job(JobRegistry *registry, pid_t pid, const char *nome_task){
   
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



void listar_jobs(JobRegistry *registry){
   
}

void atualizar_status_job(JobRegistry *registry, pid_t pid, int status){

}

void coletar_todos_jobs(JobRegistry *registry){

}

