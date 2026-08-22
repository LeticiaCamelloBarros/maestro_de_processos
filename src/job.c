#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
typedef struct job {
    int id_job; // número fácil de se decorar que facilita na hora de chamar o job 
    int PID ; //para o SO , um número grande sequencial q ninguém decora . 
    char nome_task[50]; //torna mais fácil de o usuário identificar o job . 
    bool running ; //true para running e false para não running 
    int status ; //valor 'cru' devolvido pelo wait pid 
}job ; 

