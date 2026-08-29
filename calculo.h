typedef struct {
    int largura;
    int altura;
    int max_iteracoes;
    double re_min, re_max; /* região real:      [-2.0, 1.0]  */
    double im_min, im_max; /* região imaginária: [-1.5, 1.5] */
} ParametrosMandelbrot;