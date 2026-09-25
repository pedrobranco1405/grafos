#ifndef COLORACAO_H
#define COLORACAO_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

/* Retornam um vetor alocado com a cor de cada vertice, numerada a partir
   de 0, e escrevem a quantidade de cores. O chamador deve usar free.
   Em entrada invalida ou falha de alocacao, retornam NULL e zeram
   *num_cores. O grafo vazio retorna um vetor liberavel e zero cores. */
int *coloracao_gulosa(GrafoLista *g, int *num_cores);
int *coloracao_welsh_powell(GrafoLista *g, int *num_cores);

/* Testa todos os componentes por BFS e 2-coloracao.
   Retorna 1 se bipartido, 0 se nao e -1 em erro. */
int eh_bipartido(GrafoLista *g);

#endif
