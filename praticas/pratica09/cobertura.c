#include "cobertura.h"

#include <stdlib.h>
#include <string.h>

static int entrada_valida(int n, int m, const Aresta *arestas,
                          const int *selecionados)
{
    if (n <= 0 || m < 0 || (m > 0 && arestas == NULL) || selecionados == NULL) {
        return 0;
    }
    for (int i = 0; i < m; ++i) {
        if (arestas[i].u < 0 || arestas[i].u >= n ||
            arestas[i].v < 0 || arestas[i].v >= n) {
            return 0;
        }
    }
    return 1;
}

int eh_cobertura(int n, int m, const Aresta *arestas, const int *selecionados)
{
    if (!entrada_valida(n, m, arestas, selecionados)) {
        return -1;
    }
    for (int v = 0; v < n; ++v) {
        if (selecionados[v] != 0 && selecionados[v] != 1) {
            return -1;
        }
    }
    for (int i = 0; i < m; ++i) {
        if (!selecionados[arestas[i].u] && !selecionados[arestas[i].v]) {
            return 0;
        }
    }
    return 1;
}

int cobertura_gulosa(int n, int m, const Aresta *arestas, int *selecionados)
{
    if (!entrada_valida(n, m, arestas, selecionados)) {
        return -1;
    }
    size_t *grau = calloc((size_t)n, sizeof(*grau));
    if (grau == NULL) {
        return -1;
    }
    for (int v = 0; v < n; ++v) {
        selecionados[v] = 0;
    }

    int quantidade = 0;
    while (1) {
        memset(grau, 0, (size_t)n * sizeof(*grau));
        for (int i = 0; i < m; ++i) {
            int u = arestas[i].u;
            int v = arestas[i].v;
            if (!selecionados[u] && !selecionados[v]) {
                ++grau[u];
                ++grau[v]; /* Um laco contribui com 2 para o grau. */
            }
        }
        int escolhido = -1;
        size_t maior_grau = 0;
        for (int v = 0; v < n; ++v) {
            if (grau[v] > maior_grau) {
                maior_grau = grau[v];
                escolhido = v;
            }
        }
        if (escolhido == -1) {
            break; /* Todas as arestas estao cobertas. */
        }
        /* Empates sao resolvidos pelo menor indice de vertice. */
        selecionados[escolhido] = 1;
        ++quantidade;
    }
    free(grau);
    return quantidade;
}

typedef struct {
    int n, m;
    const Aresta *arestas;
    int *atual, *melhor, *marcados;
    int tamanho_melhor;
} BuscaCobertura;

static void buscar_cobertura(BuscaCobertura *busca, int quantidade)
{
    /* Limite superior: uma solucao deste tamanho ja foi encontrada. */
    if (quantidade >= busca->tamanho_melhor) {
        return;
    }

    int u = -1, v = -1;
    int limite_inferior = 0;
    memset(busca->marcados, 0, (size_t)busca->n * sizeof(*busca->marcados));
    for (int i = 0; i < busca->m; ++i) {
        int a = busca->arestas[i].u;
        int b = busca->arestas[i].v;
        if (!busca->atual[a] && !busca->atual[b]) {
            if (u == -1) {
                u = a;
                v = b;
            }
            /* Arestas sem pontas em comum exigem vertices distintos na
             * cobertura. Seu total e um limite inferior para o que falta.
             */
            if (!busca->marcados[a] && !busca->marcados[b]) {
                busca->marcados[a] = busca->marcados[b] = 1;
                ++limite_inferior;
            }
        }
    }
    if (u == -1) {
        busca->tamanho_melhor = quantidade;
        memcpy(busca->melhor, busca->atual,
               (size_t)busca->n * sizeof(*busca->melhor));
        return;
    }
    if (limite_inferior >= busca->tamanho_melhor - quantidade) {
        return; /* Mesmo o melhor caso deste ramo nao melhora a solucao. */
    }

    /* Para cobrir a aresta descoberta (u,v), e obrigatorio escolher u ou v.
     * Explora as duas possibilidades e desfaz cada escolha (backtracking).
     */
    busca->atual[u] = 1;
    buscar_cobertura(busca, quantidade + 1);
    busca->atual[u] = 0;
    if (v != u) {
        busca->atual[v] = 1;
        buscar_cobertura(busca, quantidade + 1);
        busca->atual[v] = 0;
    }
}

int cobertura_minima_bt(int n, int m, const Aresta *arestas, int *selecionados)
{
    /* A solucao gulosa fornece um limite superior inicial valido. */
    int limite = cobertura_gulosa(n, m, arestas, selecionados);
    if (limite <= 0) {
        return limite;
    }
    int *atual = calloc((size_t)n, sizeof(*atual));
    int *marcados = calloc((size_t)n, sizeof(*marcados));
    if (atual == NULL || marcados == NULL) {
        free(atual);
        free(marcados);
        return -1;
    }
    BuscaCobertura busca = {n, m, arestas, atual, selecionados, marcados, limite};
    buscar_cobertura(&busca, 0);
    free(atual);
    free(marcados);
    return busca.tamanho_melhor;
}
