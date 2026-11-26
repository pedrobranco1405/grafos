#include "tps.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int tamanho_valido(int n)
{
    return n > 0 && (size_t)n <= SIZE_MAX / sizeof(int) / (size_t)n;
}

static int peso(int n, const int *pesos, int u, int v)
{
    return pesos[(size_t)u * n + v];
}

int grafo_para_tsp(int n, const int *adj, int *pesos)
{
    if (!tamanho_valido(n) || adj == NULL || pesos == NULL) {
        return -1;
    }
    for (int u = 0; u < n; ++u) {
        size_t linha = (size_t)u * n;
        for (int v = 0; v < n; ++v) {
            pesos[linha + v] = u == v ? 0 : (adj[linha + v] != 0 ? 1 : 2);
        }
    }
    return 1;
}

static int entrada_valida(int n, const int *pesos, int origem, const int *rota)
{
    if (!tamanho_valido(n) || pesos == NULL || rota == NULL ||
        origem < 0 || origem >= n) {
        return 0;
    }
    for (int u = 0; u < n; ++u) {
        if (peso(n, pesos, u, u) != 0) {
            return 0;
        }
        for (int v = u + 1; v < n; ++v) {
            int w = peso(n, pesos, u, v);
            if (w < 0 || w != peso(n, pesos, v, u)) {
                return 0;
            }
        }
    }
    return 1;
}

typedef struct {
    int n, origem;
    const int *pesos;
    int *ordem, *melhor_rota;
    unsigned char *visitado;
    long long melhor_custo;
} BuscaTsp;

static void permutar(BuscaTsp *busca, int tamanho, long long custo)
{
    int ultimo = busca->ordem[tamanho - 1];
    const int *pesos_ultimo = busca->pesos + (size_t)ultimo * busca->n;
    if (tamanho == busca->n) {
        custo += pesos_ultimo[busca->origem];
        if (custo < busca->melhor_custo) {
            busca->melhor_custo = custo;
            memcpy(busca->melhor_rota, busca->ordem,
                   (size_t)busca->n * sizeof(*busca->ordem));
        }
        return;
    }

    /* Percorre todas as (n - 1)! permutacoes com a origem fixa. */
    for (int v = 0; v < busca->n; ++v) {
        if (!busca->visitado[v]) {
            busca->visitado[v] = 1;
            busca->ordem[tamanho] = v;
            permutar(busca, tamanho + 1, custo + pesos_ultimo[v]);
            busca->visitado[v] = 0;
        }
    }
}

long long tsp_forca_bruta(int n, const int *pesos, int origem, int *rota)
{
    if (!entrada_valida(n, pesos, origem, rota)) {
        return -1;
    }
    int *ordem = calloc((size_t)n, sizeof(*ordem));
    unsigned char *visitado = calloc((size_t)n, sizeof(*visitado));
    if (ordem == NULL || visitado == NULL) {
        free(ordem);
        free(visitado);
        return -1;
    }
    BuscaTsp busca = {n, origem, pesos, ordem, rota, visitado, LLONG_MAX};
    ordem[0] = origem;
    visitado[origem] = 1;
    permutar(&busca, 1, 0);
    rota[n] = origem;
    free(ordem);
    free(visitado);
    return busca.melhor_custo;
}

long long tsp_vizinho_mais_proximo(int n, const int *pesos, int origem, int *rota)
{
    if (!entrada_valida(n, pesos, origem, rota)) {
        return -1;
    }
    unsigned char *visitado = calloc((size_t)n, sizeof(*visitado));
    if (visitado == NULL) {
        return -1;
    }

    long long custo = 0;
    rota[0] = origem;
    visitado[origem] = 1;
    for (int i = 1; i < n; ++i) {
        int ultimo = rota[i - 1];
        int proximo = -1, menor = 0;
        for (int v = 0; v < n; ++v) {
            int w = peso(n, pesos, ultimo, v);
            if (!visitado[v] && (proximo == -1 || w < menor)) {
                proximo = v;
                menor = w;
            }
        }
        rota[i] = proximo;
        visitado[proximo] = 1;
        custo += menor;
    }
    rota[n] = origem;
    custo += peso(n, pesos, rota[n - 1], origem);
    free(visitado);
    return custo;
}

long long tsp_insercao(int n, const int *pesos, int origem, int *rota)
{
    if (!entrada_valida(n, pesos, origem, rota)) {
        return -1;
    }
    int *distancia = calloc((size_t)n, sizeof(*distancia));
    if (distancia == NULL) {
        return -1;
    }
    for (int v = 0; v < n; ++v) {
        distancia[v] = peso(n, pesos, origem, v);
    }
    distancia[origem] = -1;
    rota[0] = rota[1] = origem;
    long long custo = 0;

    for (int tamanho = 1; tamanho < n; ++tamanho) {
        /* Seleciona o vertice de fora mais proximo de qualquer um no circuito. */
        int escolhido = -1;
        for (int v = 0; v < n; ++v) {
            if (distancia[v] >= 0 &&
                (escolhido == -1 || distancia[v] < distancia[escolhido])) {
                escolhido = v;
            }
        }

        /* Entre as arestas do circuito, escolhe a de menor aumento de custo. */
        int posicao = 1;
        long long menor_aumento = LLONG_MAX;
        for (int i = 0; i < tamanho; ++i) {
            int u = rota[i], v = rota[i + 1];
            long long aumento = (long long)peso(n, pesos, u, escolhido) +
                                peso(n, pesos, escolhido, v) - peso(n, pesos, u, v);
            if (aumento < menor_aumento) {
                menor_aumento = aumento;
                posicao = i + 1;
            }
        }
        memmove(rota + posicao + 1, rota + posicao,
                (size_t)(tamanho - posicao + 1) * sizeof(*rota));
        rota[posicao] = escolhido;
        custo += menor_aumento;

        distancia[escolhido] = -1;
        for (int v = 0; v < n; ++v) {
            int w = peso(n, pesos, escolhido, v);
            if (distancia[v] >= 0 && w < distancia[v]) {
                distancia[v] = w;
            }
        }
    }
    free(distancia);
    return custo;
}
