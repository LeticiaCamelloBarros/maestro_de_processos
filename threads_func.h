typedef struct  {
int altura ;
int largura;
int max_iteracoes;
unsigned char *matriz_pixels; // Ponteiro para a imagem/matriz na memória
}  DadosCompartilhados;

typedef struct {
    int id_thread;
    int total_threads;
    DadosCompartilhados *compartilhado; // Ponteiro para a struct compartilhada
} ParamsThread;
