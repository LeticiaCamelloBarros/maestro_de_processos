#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
long parse_positive_long(const char *arg, const char *nome) {
    errno = 0;
    char *endptr = NULL;
    long valor = strtol(arg, &endptr, 10);

    if (endptr == arg) {
        fprintf(stderr, "Erro: %s ('%s') não é um número.\n", nome, arg);
        exit(EXIT_FAILURE);
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Erro: %s ('%s') tem caracteres inválidos.\n", nome, arg);
        exit(EXIT_FAILURE);
    }
    if (errno == ERANGE || valor == LONG_MIN || valor == LONG_MAX) {
        fprintf(stderr, "Erro: %s ('%s') está fora do intervalo.\n", nome, arg);
        exit(EXIT_FAILURE);
    }
    if (valor <= 0) {
        fprintf(stderr, "Erro: %s deve ser positivo.\n", nome);
        exit(EXIT_FAILURE);
    }

    return valor;
}
