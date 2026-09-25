#include <stdlib.h>
#include "coloracao.h"

static int grafo_valido(const GrafoLista *g)
{
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL)) {
        return 0;
    }
    for (int u = 0; u < g->n; u++) {
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            int v = no->destino;
            if (v < 0 || v >= g->n || v == u) {
                return 0;
            }
            int simetricas = 0;
            for (const No *volta = g->adj[v];
                 volta != NULL; volta = volta->prox) {
                simetricas += volta->destino == u;
            }
            if (simetricas != 1) {
                return 0;
            }
        }
    }
    return 1;
}

static int *alocar_cores(int n)
{
    size_t quantidade = n > 0 ? (size_t)(unsigned int)n : 1;
    int *cores = malloc(quantidade * sizeof *cores);
    if (cores != NULL) {
        for (int u = 0; u < n; u++) {
            cores[u] = -1;
        }
    }
    return cores;
}

static int colorir_na_ordem(GrafoLista *g, const int *ordem,
                            int *cores, int *num_cores)
{
    size_t n = (size_t)(unsigned int)g->n;
    int *indisponivel = calloc(n > 0 ? n : 1, sizeof *indisponivel);
    if (indisponivel == NULL) {
        return 0;
    }

    int marcador = 0;
    for (int i = 0; i < g->n; i++) {
        int u = ordem == NULL ? i : ordem[i];
        marcador++;
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            int cor = cores[no->destino];
            if (cor >= 0) {
                indisponivel[cor] = marcador;
            }
        }
        int cor = 0;
        while (cor < g->n && indisponivel[cor] == marcador) {
            cor++;
        }
        cores[u] = cor;
        if (cor + 1 > *num_cores) {
            *num_cores = cor + 1;
        }
    }
    free(indisponivel);
    return 1;
}

int *coloracao_gulosa(GrafoLista *g, int *num_cores)
{
    if (num_cores == NULL) {
        return NULL;
    }
    *num_cores = 0;
    if (!grafo_valido(g)) {
        return NULL;
    }
    int *cores = alocar_cores(g->n);
    if (cores == NULL) {
        return NULL;
    }
    if (!colorir_na_ordem(g, NULL, cores, num_cores)) {
        free(cores);
        return NULL;
    }
    return cores;
}

static int grau(const GrafoLista *g, int u)
{
    int resultado = 0;
    for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
        resultado++;
    }
    return resultado;
}

int *coloracao_welsh_powell(GrafoLista *g, int *num_cores)
{
    if (num_cores == NULL) {
        return NULL;
    }
    *num_cores = 0;
    if (!grafo_valido(g)) {
        return NULL;
    }
    int *cores = alocar_cores(g->n);
    if (cores == NULL) {
        return NULL;
    }
    size_t n = g->n > 0 ? (size_t)(unsigned int)g->n : 1;
    int *ordem = malloc(n * sizeof *ordem);
    int *graus = malloc(n * sizeof *graus);
    if (ordem == NULL || graus == NULL) {
        free(ordem);
        free(graus);
        free(cores);
        return NULL;
    }
    for (int u = 0; u < g->n; u++) {
        ordem[u] = u;
        graus[u] = grau(g, u);
    }

    /* Selection sort deixa clara a regra de Welsh-Powell. Em empate,
       o vertice de menor indice aparece primeiro. */
    for (int i = 0; i < g->n; i++) {
        int melhor = i;
        for (int j = i + 1; j < g->n; j++) {
            int atual = ordem[j];
            int escolhido = ordem[melhor];
            if (graus[atual] > graus[escolhido] ||
                (graus[atual] == graus[escolhido] && atual < escolhido)) {
                melhor = j;
            }
        }
        int temporario = ordem[i];
        ordem[i] = ordem[melhor];
        ordem[melhor] = temporario;
    }

    if (!colorir_na_ordem(g, ordem, cores, num_cores)) {
        free(ordem);
        free(graus);
        free(cores);
        return NULL;
    }
    free(ordem);
    free(graus);
    return cores;
}

int eh_bipartido(GrafoLista *g)
{
    if (!grafo_valido(g)) {
        return -1;
    }
    if (g->n == 0) {
        return 1;
    }
    size_t n = (size_t)(unsigned int)g->n;
    int *cor = malloc(n * sizeof *cor);
    int *fila = malloc(n * sizeof *fila);
    if (cor == NULL || fila == NULL) {
        free(cor);
        free(fila);
        return -1;
    }
    for (int u = 0; u < g->n; u++) {
        cor[u] = -1;
    }

    for (int origem = 0; origem < g->n; origem++) {
        if (cor[origem] != -1) {
            continue;
        }
        int inicio = 0, fim = 0;
        cor[origem] = 0;
        fila[fim++] = origem;
        while (inicio < fim) {
            int u = fila[inicio++];
            for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
                int v = no->destino;
                if (cor[v] == -1) {
                    cor[v] = 1 - cor[u];
                    fila[fim++] = v;
                } else if (cor[v] == cor[u]) {
                    free(cor);
                    free(fila);
                    return 0;
                }
            }
        }
    }
    free(cor);
    free(fila);
    return 1;
}
