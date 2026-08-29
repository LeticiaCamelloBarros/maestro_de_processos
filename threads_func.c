#include <pthread.h>
#include <omp.h>
#include <calculo.h>
//pthread_t indentificador de uma thread - especifico da biblioteca pthread 

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


 
/* ------------------------------------------------------------------ */
/* Função auxiliar (não pedida explicitamente, mas necessária):        */
/* aplica z_novo = z_atual^2 + c e devolve o número de iterações       */
/* até o ponto "explodir" ou atingir o máximo.                         */
/* ------------------------------------------------------------------ */
static int calcula_iteracoes(double c_re, double c_im, int max_iteracoes) {
    double z_re = 0.0, z_im = 0.0;
    int iter = 0;
 
    while (z_re * z_re + z_im * z_im <= 4.0 && iter < max_iteracoes) {
        double z_re_novo = z_re * z_re - z_im * z_im + c_re;
        z_im = 2.0 * z_re * z_im + c_im;
        z_re = z_re_novo;
        iter++;
    }
    return iter;
}
 
/* ------------------------------------------------------------------ */
/* 1) Definir manualmente o número de threads (Seção 4.5.3).           */
/*                                                                      */
/* O livro explica que OpenMP permite ao desenvolvedor escolher         */
/* o nível de paralelismo, inclusive definindo manualmente quantas      */
/* threads serão usadas em vez de deixar o run-time decidir com base    */
/* no número de núcleos. Aqui usamos omp_set_num_threads(), que vale    */
/* para todas as regiões paralelas seguintes até ser chamada de novo.   */
/* ------------------------------------------------------------------ */
void definir_numero_threads(int num_threads) {
    if (num_threads <= 0) {
        fprintf(stderr, "Erro: numero de threads invalido (%d)\n", num_threads);
        exit(EXIT_FAILURE);
    }
    omp_set_num_threads(num_threads);
}
 
/* ------------------------------------------------------------------ */
/* 2) Criar threads (Seção 4.5, introdução / 4.5.3).                   */
/*                                                                      */
/* Diferente de Pthreads, em OpenMP não existe uma chamada explícita    */
/* equivalente a pthread_create(): a criação das threads acontece       */
/* quando o run-time encontra a diretiva "#pragma omp parallel".        */
/* Essa diretiva cria automaticamente uma thread para cada núcleo       */
/* disponível (ou para o número definido em definir_numero_threads()),  */
/* todas executando o mesmo bloco de código simultaneamente. Ao sair    */
/* do bloco, as threads são encerradas (join implícito).                */
/* ------------------------------------------------------------------ */
void criar_threads(void) {
    #pragma omp parallel
    {
        int id_thread = omp_get_thread_num();
        int total_threads = omp_get_num_threads();
 
        /* Uso apenas ilustrativo: o enunciado da atividade proíbe        */
        /* impressão em stdout durante a execução normal do programa.     */
        /* Em produção, substitua por trabalho real dentro da região.     */
        (void) id_thread;
        (void) total_threads;
    }
}
 
/* ------------------------------------------------------------------ */
/* 3) Dividir as iterações entre as threads (Seção 4.5.3).             */
/*                                                                      */
/* "#pragma omp parallel for" pega um laço sequencial e divide as       */
/* iterações automaticamente entre as threads criadas -- sem que o      */
/* programador precise calcular manualmente qual faixa cada thread      */
/* processa (diferente do que é feito "na mão" com Pthreads).           */
/* Aqui dividimos as LINHAS da imagem entre as threads. A cláusula       */
/* schedule(static) faz a divisão em blocos contíguos e de tamanho       */
/* fixo, o equivalente mais próximo de particionar manualmente o         */
/* trabalho em faixas, como se faria com Pthreads.                      */
/* ------------------------------------------------------------------ */
void dividir_iteracoes_entre_threads(int **imagem, const ParametrosMandelbrot *p) {
    int linha, coluna;
    double delta_re = (p->re_max - p->re_min) / p->largura;
    double delta_im = (p->im_max - p->im_min) / p->altura;
 
    #pragma omp parallel for schedule(static) private(coluna)
    for (linha = 0; linha < p->altura; linha++) {
        double c_im = p->im_min + linha * delta_im;
 
        for (coluna = 0; coluna < p->largura; coluna++) {
            double c_re = p->re_min + coluna * delta_re;
            int iteracoes = calcula_iteracoes(c_re, c_im, p->max_iteracoes);
            imagem[linha][coluna] = iteracoes;
        }
    }
}
 
/* ------------------------------------------------------------------ */
/* 4) Percorrer as colunas da imagem do Mandelbrot (Seção 4.5.3).      */
/*                                                                      */
/* Mesma ideia da função anterior, mas paralelizando pelo eixo das      */
/* COLUNAS em vez das linhas -- útil para ter uma segunda estratégia    */
/* de divisão de trabalho, como pedido no enunciado da atividade        */
/* (duas implementações distintas de particionamento). A cláusula       */
/* "private(linha)" garante que cada thread tenha sua própria cópia     */
/* da variável de laço interno, evitando condição de corrida.           */
/* ------------------------------------------------------------------ */
void percorrer_colunas_mandelbrot(int **imagem, const ParametrosMandelbrot *p) {
    int coluna, linha;
    double delta_re = (p->re_max - p->re_min) / p->largura;
    double delta_im = (p->im_max - p->im_min) / p->altura;
 
    #pragma omp parallel for schedule(static) private(linha)
    for (coluna = 0; coluna < p->largura; coluna++) {
        double c_re = p->re_min + coluna * delta_re;
 
        for (linha = 0; linha < p->altura; linha++) {
            double c_im = p->im_min + linha * delta_im;
            int iteracoes = calcula_iteracoes(c_re, c_im, p->max_iteracoes);
            imagem[linha][coluna] = iteracoes;
        }
    }
}
 