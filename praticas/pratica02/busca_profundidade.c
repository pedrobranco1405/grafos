#include <stdlib.h>
#include "busca_profundidade.h"

int inicializar_pilha(Pilha *pilha, int capacidade)
{
    if (pilha == NULL) {
        return 0;
    }
    *pilha = (Pilha){.topo = -1};
    if (capacidade <= 0) {
        return 0;
    }
    pilha->dados = calloc((size_t)capacidade, sizeof *pilha->dados);
    if (pilha->dados == NULL) {
        return 0;
    }
    pilha->capacidade = capacidade;
    return 1;
}

int empilhar(Pilha *pilha, int valor)
{
    if (pilha == NULL || pilha->dados == NULL ||
        pilha->topo == pilha->capacidade - 1) {
        return 0;
    }
    pilha->dados[++pilha->topo] = valor;
    return 1;
}

int desempilhar(Pilha *pilha, int *valor)
{
    if (pilha == NULL || pilha->dados == NULL || valor == NULL ||
        pilha->topo < 0) {
        return 0;
    }
    *valor = pilha->dados[pilha->topo--];
    return 1;
}

void liberar_pilha(Pilha *pilha)
{
    if (pilha != NULL) {
        free(pilha->dados);
        *pilha = (Pilha){.topo = -1};
    }
}

void dfs_recursiva(GrafoLista *g, int u, int *visitado,
                   int *entrada, int *saida, int *tempo)
{
    if (g == NULL || g->adj == NULL || u < 0 || u >= g->n ||
        visitado == NULL || entrada == NULL || saida == NULL ||
        tempo == NULL || visitado[u]) {
        return;
    }

    visitado[u] = 1;
    entrada[u] = ++(*tempo);
    for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
        if (!visitado[no->destino]) {
            dfs_recursiva(g, no->destino, visitado, entrada, saida, tempo);
        }
    }
    saida[u] = ++(*tempo);
}

static void visitar_componente(GrafoLista *g, int origem,
                               int *visitado, Pilha *pilha)
{
    visitado[origem] = 1;
    empilhar(pilha, origem);
    int u;
    while (desempilhar(pilha, &u)) {
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            int v = no->destino;
            if (!visitado[v]) {
                /* Cada vertice ocupa a pilha no maximo uma vez. */
                visitado[v] = 1;
                empilhar(pilha, v);
            }
        }
    }
}

int dfs_iterativa(GrafoLista *g, int origem, int *visitado)
{
    if (g == NULL || g->adj == NULL || origem < 0 || origem >= g->n ||
        visitado == NULL) {
        return 0;
    }
    if (visitado[origem]) {
        return 1;
    }
    Pilha pilha;
    if (!inicializar_pilha(&pilha, g->n)) {
        return 0;
    }
    visitar_componente(g, origem, visitado, &pilha);
    liberar_pilha(&pilha);
    return 1;
}

int contar_componentes(GrafoLista *g)
{
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL)) {
        return -1;
    }
    if (g->n == 0) {
        return 0;
    }
    int *visitado = calloc((size_t)g->n, sizeof *visitado);
    if (visitado == NULL) {
        return -1;
    }
    Pilha pilha;
    if (!inicializar_pilha(&pilha, g->n)) {
        free(visitado);
        return -1;
    }

    int componentes = 0;
    for (int u = 0; u < g->n; u++) {
        if (!visitado[u]) {
            componentes++;
            visitar_componente(g, u, visitado, &pilha);
        }
    }
    liberar_pilha(&pilha);
    free(visitado);
    return componentes;
}

static int ciclo_recursivo(GrafoLista *g, int u, int pai, int *visitado)
{
    visitado[u] = 1;
    for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
        int v = no->destino;
        if (!visitado[v]) {
            if (ciclo_recursivo(g, v, u, visitado)) {
                return 1;
            }
        } else if (v != pai) {
            /* A volta ao pai e a mesma aresta, e nao constitui um ciclo. */
            return 1;
        }
    }
    return 0;
}

int tem_ciclo(GrafoLista *g)
{
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL)) {
        return -1;
    }
    if (g->n == 0) {
        return 0;
    }
    int *visitado = calloc((size_t)g->n, sizeof *visitado);
    if (visitado == NULL) {
        return -1;
    }
    for (int u = 0; u < g->n; u++) {
        if (!visitado[u] && ciclo_recursivo(g, u, -1, visitado)) {
            free(visitado);
            return 1;
        }
    }
    free(visitado);
    return 0;
}
