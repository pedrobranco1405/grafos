#ifndef DAG_H
#define DAG_H

/* Grafo direcionado: cada no em adj[u] representa somente o arco u -> v.
   Vertices numerados de 0 a n - 1; n = 0 representa um grafo vazio. */
typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

/* Retornam um vetor alocado, que deve ser liberado com free, e escrevem
   n em *tamanho. Em ciclo, entrada invalida ou falha de alocacao retornam
   NULL e zeram *tamanho, quando esse ponteiro nao e NULL.
   O grafo vazio retorna um vetor liberavel com tamanho 0.
   Os algoritmos nao modificam o grafo e podem produzir ordens diferentes. */
int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho);
int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho);

/* Retorna 1 se aciclico, 0 se houver ciclo e -1 em entrada invalida
   ou falha de alocacao. O grafo vazio e um DAG. */
int eh_dag(GrafoLista *g);

#endif
