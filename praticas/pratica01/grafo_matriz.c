#include <stdlib.h>
#include "grafo_matriz.h"

static int vertice_valido(const GrafoMatriz *grafo, int v)
{
    return grafo != NULL && v >= 0 && v < grafo->n;
}

GrafoMatriz *criar_grafo_matriz(int n)
{
    if (n <= 0) {
        return NULL;
    }

    GrafoMatriz *grafo = malloc(sizeof *grafo);
    if (grafo == NULL) {
        return NULL;
    }

    grafo->n = n;
    grafo->adj = calloc((size_t)n, sizeof *grafo->adj);
    if (grafo->adj == NULL) {
        free(grafo);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        grafo->adj[i] = calloc((size_t)n, sizeof *grafo->adj[i]);
        if (grafo->adj[i] == NULL) {
            /* As linhas ainda nao alocadas sao NULL, gracas ao calloc. */
            liberar_grafo_matriz(grafo);
            return NULL;
        }
    }

    return grafo;
}

int inserir_aresta_matriz(GrafoMatriz *grafo, int u, int v)
{
    if (!vertice_valido(grafo, u) || !vertice_valido(grafo, v) || u == v) {
        return -1;
    }
    if (grafo->adj[u][v] != 0) {
        return 0;
    }

    grafo->adj[u][v] = 1;
    grafo->adj[v][u] = 1;
    return 1;
}

int remover_aresta_matriz(GrafoMatriz *grafo, int u, int v)
{
    if (!vertice_valido(grafo, u) || !vertice_valido(grafo, v) || u == v) {
        return -1;
    }
    if (grafo->adj[u][v] == 0) {
        return 0;
    }

    grafo->adj[u][v] = 0;
    grafo->adj[v][u] = 0;
    return 1;
}

int grau_matriz(const GrafoMatriz *grafo, int v)
{
    if (!vertice_valido(grafo, v)) {
        return -1;
    }

    int grau = 0;
    for (int i = 0; i < grafo->n; i++) {
        grau += grafo->adj[v][i];
    }
    return grau;
}

int sao_adjacentes_matriz(const GrafoMatriz *grafo, int u, int v)
{
    if (!vertice_valido(grafo, u) || !vertice_valido(grafo, v)) {
        return 0;
    }

    return grafo->adj[u][v] != 0;
}

void liberar_grafo_matriz(GrafoMatriz *grafo)
{
    if (grafo == NULL) {
        return;
    }

    for (int i = 0; i < grafo->n; i++) {
        free(grafo->adj[i]);
    }
    free(grafo->adj);
    free(grafo);
}
