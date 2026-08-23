//exatamente o mesmo padrão que task.c e task.h : uma struct menor representando o job e uma maior guardando as informações 
//sobre os jobs .
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_JOBS 100
//para evitar que inclua o conteúdo do header 2x 
#ifndef JOB_H
#define JOB_H
typedef enum {
    JOB_RUNNING , 
    JOB_DONE  ,
    JOB_SIGNALED , 
}Jobstate;
typedef struct job {
    int job_id; // número fácil de se decorar que facilita na hora de chamar o job 
    pid_t PID ; //para o SO , um número grande sequencial q ninguém decora tirado da biblioteca unistd ou da sys/types( uma das duas)
    char nome_task[50]; //torna mais fácil de o usuário identificar o job . 
    Jobstate state ;// resultado da tradução do status para o entendimento humano através de funções especiais 
    int status ;  //valor 'cru' devolvido pelo waitpid 
}Job ; 
typedef struct {
    Job jobs[MAX_JOBS];
    int num_jobs;
    int next_job_id;   // contador pra gerar [1], [2], [3]...
} JobRegistry;
// job.h — assinaturas

// Cria um novo job na lista, devolve o job_id gerado
int cadastrar_job(JobRegistry *jreg, pid_t pid, const char *nome_task);

// Procura um job pelo id (usado por 'wait <jobId>')
Job* buscar_job(JobRegistry *jreg, int job_id);

// Imprime todos os jobs (comando 'jobs')
void listar_jobs(JobRegistry *jreg);

// Atualiza o status de um job específico depois que ele termina
void atualizar_status_job(JobRegistry *jreg, pid_t pid, int status);

// Espera todos os jobs pendentes terminarem (usado no 'exit', pra não deixar zumbi)
void coletar_todos_jobs(JobRegistry *jreg);

#endif
