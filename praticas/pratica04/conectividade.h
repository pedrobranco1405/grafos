#ifndef CONECTIVIDADE_H
#define CONECTIVIDADE_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

typedef struct {
    int u;
    int v;
} Ponte;

/* Passo recursivo de Tarjan. Antes da primeira chamada, descoberta e low
   devem conter -1, articulacao deve conter 0 e *tempo deve ser 0.
   O grafo deve ser simples, nao orientado e ter vertices de 0 a n - 1. */
void dfs_articulacoes(GrafoLista *g, int u, int pai, int *tempo,
                      int *descoberta, int *low, int *articulacao);

/* Preenche articulacao[0..n-1] com 0 ou 1 e retorna a quantidade de
   vertices de corte. Retorna -1 se a entrada for invalida. */
int detectar_articulacoes(GrafoLista *g, int *articulacao);

/* Retorna um vetor alocado com as pontes e escreve sua quantidade.
   O chamador deve liberar o vetor com free. Retorna NULL em erro;
   para um grafo valido sem pontes, retorna um vetor liberavel e quantidade 0. */
Ponte *detectar_pontes(GrafoLista *g, int *quantidade);

#endif
