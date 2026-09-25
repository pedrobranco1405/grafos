#include <stdlib.h>
#include "busca_largura.h"

int inicializar_fila(Fila *fila, int capacidade)
{
    if (fila == NULL) {
        return 0;
    }
    *fila = (Fila){0};
    if (capacidade <= 0) {
        return 0;
    }
    fila->dados = calloc((size_t)capacidade, sizeof *fila->dados);
    if (fila->dados == NULL) {
        return 0;
    }
    fila->capacidade = capacidade;
    return 1;
}

int enfileirar(Fila *fila, int valor)
{
    if (fila == NULL || fila->dados == NULL ||
        fila->tamanho == fila->capacidade) {
        return 0;
    }
    fila->dados[fila->fim] = valor;
    fila->fim = (fila->fim + 1) % fila->capacidade;
    fila->tamanho++;
    return 1;
}

int desenfileirar(Fila *fila, int *valor)
{
    if (fila == NULL || fila->dados == NULL || valor == NULL ||
        fila->tamanho == 0) {
        return 0;
    }
    *valor = fila->dados[fila->inicio];
    fila->inicio = (fila->inicio + 1) % fila->capacidade;
    fila->tamanho--;
    return 1;
}

void liberar_fila(Fila *fila)
{
    if (fila != NULL) {
        free(fila->dados);
        *fila = (Fila){0};
    }
}

int bfs(GrafoLista *g, int origem, int *dist, int *pred)
{
    if (g == NULL || g->adj == NULL || origem < 0 || origem >= g->n ||
        dist == NULL || pred == NULL) {
        return 0;
    }
    for (int v = 0; v < g->n; v++) {
        dist[v] = -1;
        pred[v] = -1;
    }

    Fila fila;
    if (!inicializar_fila(&fila, g->n)) {
        return 0;
    }
    dist[origem] = 0;
    enfileirar(&fila, origem);

    int u;
    while (desenfileirar(&fila, &u)) {
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            int v = no->destino;
            if (dist[v] == -1) {
                /* Marcar na descoberta garante uma entrada por vertice. */
                dist[v] = dist[u] + 1;
                pred[v] = u;
                enfileirar(&fila, v);
            }
        }
    }
    liberar_fila(&fila);
    return 1;
}

int eh_bipartido(GrafoLista *g)
{
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL)) {
        return -1;
    }
    if (g->n == 0) {
        return 1;
    }

    int *cor = calloc((size_t)g->n, sizeof *cor);
    Fila fila;
    if (cor == NULL) {
        return -1;
    }
    if (!inicializar_fila(&fila, g->n)) {
        free(cor);
        return -1;
    }
    for (int v = 0; v < g->n; v++) {
        cor[v] = -1;
    }

    for (int origem = 0; origem < g->n; origem++) {
        if (cor[origem] != -1) {
            continue;
        }
        cor[origem] = 0;
        enfileirar(&fila, origem);
        int u;
        while (desenfileirar(&fila, &u)) {
            for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
                int v = no->destino;
                if (cor[v] == -1) {
                    cor[v] = 1 - cor[u];
                    enfileirar(&fila, v);
                } else if (cor[v] == cor[u]) {
                    liberar_fila(&fila);
                    free(cor);
                    return 0;
                }
            }
        }
    }
    liberar_fila(&fila);
    free(cor);
    return 1;
}
