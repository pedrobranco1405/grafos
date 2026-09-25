#include <stdlib.h>
#include "conectividade.h"

static int grafo_valido(const GrafoLista *g)
{
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL)) {
        return 0;
    }
    for (int u = 0; u < g->n; u++) {
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            if (no->destino < 0 || no->destino >= g->n || no->destino == u) {
                return 0;
            }
            int repeticoes = 0;
            for (const No *volta = g->adj[no->destino];
                 volta != NULL; volta = volta->prox) {
                repeticoes += volta->destino == u;
            }
            if (repeticoes != 1) {
                return 0;
            }
        }
    }
    return 1;
}

static int minimo(int a, int b)
{
    return a < b ? a : b;
}

static void dfs_articulacoes_impl(GrafoLista *g, int u, int pai, int *tempo,
                                  int *descoberta, int *low,
                                  int *articulacao)
{
    descoberta[u] = low[u] = ++(*tempo);
    int filhos = 0;
    for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
        int v = no->destino;
        if (descoberta[v] == -1) {
            filhos++;
            dfs_articulacoes_impl(g, v, u, tempo, descoberta, low,
                                  articulacao);
            low[u] = minimo(low[u], low[v]);

            if (pai == -1 && filhos > 1) {
                articulacao[u] = 1;
            }
            if (pai != -1 && low[v] >= descoberta[u]) {
                articulacao[u] = 1;
            }
        } else if (v != pai) {
            low[u] = minimo(low[u], descoberta[v]);
        }
    }
}

void dfs_articulacoes(GrafoLista *g, int u, int pai, int *tempo,
                      int *descoberta, int *low, int *articulacao)
{
    if (!grafo_valido(g) || u < 0 || u >= g->n || tempo == NULL ||
        descoberta == NULL || low == NULL || articulacao == NULL ||
        descoberta[u] != -1) {
        return;
    }
    dfs_articulacoes_impl(g, u, pai, tempo, descoberta, low, articulacao);
}

int detectar_articulacoes(GrafoLista *g, int *articulacao)
{
    if (!grafo_valido(g) || (g->n > 0 && articulacao == NULL)) {
        return -1;
    }
    if (g->n == 0) {
        return 0;
    }

    size_t n = (size_t)(unsigned int)g->n;
    int *descoberta = malloc(n * sizeof *descoberta);
    int *low = malloc(n * sizeof *low);
    if (descoberta == NULL || low == NULL) {
        free(descoberta);
        free(low);
        return -1;
    }
    for (int u = 0; u < g->n; u++) {
        descoberta[u] = -1;
        low[u] = -1;
        articulacao[u] = 0;
    }

    int tempo = 0;
    for (int u = 0; u < g->n; u++) {
        if (descoberta[u] == -1) {
            dfs_articulacoes(g, u, -1, &tempo, descoberta, low, articulacao);
        }
    }

    int quantidade = 0;
    for (int u = 0; u < g->n; u++) {
        quantidade += articulacao[u];
    }
    free(descoberta);
    free(low);
    return quantidade;
}

static void dfs_pontes(GrafoLista *g, int u, int pai, int *tempo,
                       int *descoberta, int *low, Ponte *pontes,
                       int *quantidade)
{
    descoberta[u] = low[u] = ++(*tempo);
    for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
        int v = no->destino;
        if (descoberta[v] == -1) {
            dfs_pontes(g, v, u, tempo, descoberta, low, pontes, quantidade);
            low[u] = minimo(low[u], low[v]);
            if (low[v] > descoberta[u]) {
                pontes[*quantidade].u = u < v ? u : v;
                pontes[*quantidade].v = u < v ? v : u;
                (*quantidade)++;
            }
        } else if (v != pai) {
            low[u] = minimo(low[u], descoberta[v]);
        }
    }
}

Ponte *detectar_pontes(GrafoLista *g, int *quantidade)
{
    if (quantidade == NULL) {
        return NULL;
    }
    *quantidade = 0;
    if (!grafo_valido(g)) {
        return NULL;
    }

    size_t capacidade = g->n > 0 ? (size_t)(unsigned int)g->n : 1;
    Ponte *pontes = malloc(capacidade * sizeof *pontes);
    if (pontes == NULL || g->n == 0) {
        return pontes;
    }
    size_t n = (size_t)(unsigned int)g->n;
    int *descoberta = malloc(n * sizeof *descoberta);
    int *low = malloc(n * sizeof *low);
    if (descoberta == NULL || low == NULL) {
        free(descoberta);
        free(low);
        free(pontes);
        return NULL;
    }
    for (int u = 0; u < g->n; u++) {
        descoberta[u] = -1;
        low[u] = -1;
    }

    int tempo = 0;
    for (int u = 0; u < g->n; u++) {
        if (descoberta[u] == -1) {
            dfs_pontes(g, u, -1, &tempo, descoberta, low,
                       pontes, quantidade);
        }
    }
    free(descoberta);
    free(low);
    return pontes;
}
