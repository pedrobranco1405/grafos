#include <stdlib.h>
#include "grafo_lista.h"

static int vertice_valido(const GrafoLista *grafo, int v)
{
    return grafo != NULL && v >= 0 && v < grafo->n;
}

GrafoLista *criar_grafo_lista(int n)
{
    if (n <= 0) {
        return NULL;
    }

    GrafoLista *grafo = malloc(sizeof *grafo);
    if (grafo == NULL) {
        return NULL;
    }

    grafo->n = n;
    grafo->adj = calloc((size_t)n, sizeof *grafo->adj);
    if (grafo->adj == NULL) {
        free(grafo);
        return NULL;
    }

    return grafo;
}

int inserir_aresta_lista(GrafoLista *grafo, int u, int v)
{
    if (!vertice_valido(grafo, u) || !vertice_valido(grafo, v) || u == v) {
        return -1;
    }
    if (sao_adjacentes_lista(grafo, u, v)) {
        return 0;
    }

    No *ida = malloc(sizeof *ida);
    if (ida == NULL) {
        return -1;
    }
    No *volta = malloc(sizeof *volta);
    if (volta == NULL) {
        free(ida);
        return -1;
    }

    /* So altera as listas depois de alocar os dois sentidos da aresta. */
    ida->destino = v;
    ida->prox = grafo->adj[u];
    volta->destino = u;
    volta->prox = grafo->adj[v];
    grafo->adj[u] = ida;
    grafo->adj[v] = volta;
    return 1;
}

static void remover_no(No **inicio, int destino)
{
    No *anterior = NULL;
    No *atual = *inicio;

    while (atual != NULL && atual->destino != destino) {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == NULL) {
        return;
    }

    if (anterior == NULL) {
        *inicio = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }
    free(atual);
}

int remover_aresta_lista(GrafoLista *grafo, int u, int v)
{
    if (!vertice_valido(grafo, u) || !vertice_valido(grafo, v) || u == v) {
        return -1;
    }
    if (!sao_adjacentes_lista(grafo, u, v)) {
        return 0;
    }

    remover_no(&grafo->adj[u], v);
    remover_no(&grafo->adj[v], u);
    return 1;
}

int grau_lista(const GrafoLista *grafo, int v)
{
    if (!vertice_valido(grafo, v)) {
        return -1;
    }

    int grau = 0;
    for (const No *no = grafo->adj[v]; no != NULL; no = no->prox) {
        grau++;
    }
    return grau;
}

int sao_adjacentes_lista(const GrafoLista *grafo, int u, int v)
{
    if (!vertice_valido(grafo, u) || !vertice_valido(grafo, v)) {
        return 0;
    }

    for (const No *no = grafo->adj[u]; no != NULL; no = no->prox) {
        if (no->destino == v) {
            return 1;
        }
    }
    return 0;
}

void liberar_grafo_lista(GrafoLista *grafo)
{
    if (grafo == NULL) {
        return;
    }

    for (int i = 0; i < grafo->n; i++) {
        No *no = grafo->adj[i];
        while (no != NULL) {
            No *proximo = no->prox;
            free(no);
            no = proximo;
        }
    }
    free(grafo->adj);
    free(grafo);
}
