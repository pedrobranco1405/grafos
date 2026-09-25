#ifndef GRAFO_MATRIZ_H
#define GRAFO_MATRIZ_H

typedef struct {
    int n;
    int **adj;
} GrafoMatriz;

/* Grafo simples, nao orientado, com vertices de 0 a n - 1. */
/* Retorna NULL se n <= 0 ou se a alocacao falhar. */
GrafoMatriz *criar_grafo_matriz(int n);

/* Retorno: 1 = alterou; 0 = ja estava assim; -1 = entrada invalida.
   Lacos (u == v) sao invalidos. Arestas repetidas nao sao inseridas. */
int inserir_aresta_matriz(GrafoMatriz *grafo, int u, int v);
int remover_aresta_matriz(GrafoMatriz *grafo, int u, int v);

/* Retorna -1 para grafo ou vertice invalido. */
int grau_matriz(const GrafoMatriz *grafo, int v);
/* Retorna 1 se ha aresta; 0 se nao ha ou se a entrada e invalida. */
int sao_adjacentes_matriz(const GrafoMatriz *grafo, int u, int v);
void liberar_grafo_matriz(GrafoMatriz *grafo);

#endif
