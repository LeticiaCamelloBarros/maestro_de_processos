#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char *argv[]) {

    if (argc != 5) {
        fprintf(stderr, "Uso: %s [largura] [altura] [max_iteracoes] [num_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    long largura       = parse_positive_long(argv[1], "largura");
    long altura        = parse_positive_long(argv[2], "altura");
    long max_iteracoes = parse_positive_long(argv[3], "max_iteracoes");
    long num_threads   = parse_positive_long(argv[4], "num_threads");
     
    // largura, altura, max_iteracoes e num_threads já estão validados aqui

    return EXIT_SUCCESS;
}