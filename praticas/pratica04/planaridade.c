#include <stdint.h>
#include <stdlib.h>
#include "planaridade.h"

enum { LIMITE_KURATOWSKI = 10, MAX_PARES = 10 };

typedef struct {
    int origem;
    int destino;
} Par;

typedef struct {
    int n;
    uint16_t adj[LIMITE_KURATOWSKI];
    uint16_t ramos;
    Par pares[MAX_PARES];
    int quantidade_pares;
} BuscaCaminhos;

static int criar_matriz(const GrafoLista *g, uint16_t *adj)
{
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL) ||
        g->n > LIMITE_KURATOWSKI) {
        return 0;
    }
    for (int u = 0; u < g->n; u++) {
        adj[u] = 0;
    }
    for (int u = 0; u < g->n; u++) {
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            int v = no->destino;
            if (v < 0 || v >= g->n || v == u ||
                (adj[u] & (uint16_t)(1u << v)) != 0) {
                return 0;
            }
            adj[u] |= (uint16_t)(1u << v);
        }
    }
    /* As funcoes desta pratica recebem grafos nao orientados. */
    for (int u = 0; u < g->n; u++) {
        for (int v = u + 1; v < g->n; v++) {
            int uv = (adj[u] & (uint16_t)(1u << v)) != 0;
            int vu = (adj[v] & (uint16_t)(1u << u)) != 0;
            if (uv != vu) {
                return 0;
            }
        }
    }
    return 1;
}

static int validar_sem_limite(const GrafoLista *g, long long *arestas)
{
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL)) {
        return 0;
    }
    long long incidencias = 0;
    for (int u = 0; u < g->n; u++) {
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            int v = no->destino;
            if (v < 0 || v >= g->n || v == u) {
                return 0;
            }
            int simetricas = 0;
            for (const No *volta = g->adj[v]; volta != NULL; volta = volta->prox) {
                simetricas += volta->destino == u;
            }
            if (simetricas != 1) {
                return 0;
            }
            incidencias++;
        }
    }
    if ((incidencias & 1) != 0) {
        return 0;
    }
    *arestas = incidencias / 2;
    return 1;
}

int eh_planar_euler(GrafoLista *g)
{
    long long m;
    if (!validar_sem_limite(g, &m)) {
        return -1;
    }
    if (g->n < 3) {
        return 1;
    }
    return m <= 3LL * g->n - 6;
}

static int buscar_pares(BuscaCaminhos *busca, int indice,
                        uint16_t internos_usados);

static int explorar_caminho(BuscaCaminhos *busca, int indice, int atual,
                            int destino, uint16_t visitados,
                            uint16_t internos, uint16_t internos_usados)
{
    uint16_t vizinhos = busca->adj[atual];
    for (int v = 0; v < busca->n; v++) {
        uint16_t bit = (uint16_t)(1u << v);
        if ((vizinhos & bit) == 0) {
            continue;
        }
        if (v == destino) {
            if (buscar_pares(busca, indice + 1,
                             (uint16_t)(internos_usados | internos))) {
                return 1;
            }
        } else if ((visitados & bit) == 0 &&
                   (busca->ramos & bit) == 0 &&
                   (internos_usados & bit) == 0) {
            if (explorar_caminho(busca, indice, v, destino,
                                 (uint16_t)(visitados | bit),
                                 (uint16_t)(internos | bit),
                                 internos_usados)) {
                return 1;
            }
        }
    }
    return 0;
}

static int buscar_pares(BuscaCaminhos *busca, int indice,
                        uint16_t internos_usados)
{
    if (indice == busca->quantidade_pares) {
        return 1;
    }
    int origem = busca->pares[indice].origem;
    int destino = busca->pares[indice].destino;
    return explorar_caminho(busca, indice, origem, destino,
                            (uint16_t)(1u << origem), 0, internos_usados);
}

static int testar_k5(BuscaCaminhos *busca, uint16_t ramos)
{
    busca->ramos = ramos;
    busca->quantidade_pares = 0;
    for (int u = 0; u < busca->n; u++) {
        if ((ramos & (uint16_t)(1u << u)) == 0) {
            continue;
        }
        for (int v = u + 1; v < busca->n; v++) {
            if ((ramos & (uint16_t)(1u << v)) != 0) {
                busca->pares[busca->quantidade_pares++] = (Par){u, v};
            }
        }
    }
    return buscar_pares(busca, 0, 0);
}

int contem_subdivisao_k5(GrafoLista *g)
{
    BuscaCaminhos busca = {0};
    if (!criar_matriz(g, busca.adj)) {
        return -1;
    }
    busca.n = g->n;
    if (g->n < 5) {
        return 0;
    }
    uint16_t limite = (uint16_t)(1u << g->n);
    for (uint16_t conjunto = 0; conjunto < limite; conjunto++) {
        int quantidade = 0;
        for (int v = 0; v < g->n; v++) {
            quantidade += (conjunto >> v) & 1u;
        }
        if (quantidade == 5 && testar_k5(&busca, conjunto)) {
            return 1;
        }
    }
    return 0;
}

static int testar_k33(BuscaCaminhos *busca, uint16_t ramos,
                      uint16_t esquerda)
{
    uint16_t direita = (uint16_t)(ramos & ~esquerda);
    busca->ramos = ramos;
    busca->quantidade_pares = 0;
    for (int u = 0; u < busca->n; u++) {
        if ((esquerda & (uint16_t)(1u << u)) == 0) {
            continue;
        }
        for (int v = 0; v < busca->n; v++) {
            if ((direita & (uint16_t)(1u << v)) != 0) {
                busca->pares[busca->quantidade_pares++] = (Par){u, v};
            }
        }
    }
    return buscar_pares(busca, 0, 0);
}

int contem_subdivisao_k33(GrafoLista *g)
{
    BuscaCaminhos busca = {0};
    if (!criar_matriz(g, busca.adj)) {
        return -1;
    }
    busca.n = g->n;
    if (g->n < 6) {
        return 0;
    }
    uint16_t limite = (uint16_t)(1u << g->n);
    for (uint16_t ramos = 0; ramos < limite; ramos++) {
        int quantidade = 0, primeiro = -1;
        for (int v = 0; v < g->n; v++) {
            if ((ramos & (uint16_t)(1u << v)) != 0) {
                quantidade++;
                if (primeiro == -1) {
                    primeiro = v;
                }
            }
        }
        if (quantidade != 6) {
            continue;
        }
        /* Fixar o menor ramo na esquerda elimina a troca simetrica
           entre as duas partes de K3,3. */
        for (uint16_t esquerda = ramos; esquerda != 0;
             esquerda = (uint16_t)((esquerda - 1) & ramos)) {
            int tamanho = 0;
            for (int v = 0; v < g->n; v++) {
                tamanho += (esquerda >> v) & 1u;
            }
            if (tamanho == 3 &&
                (esquerda & (uint16_t)(1u << primeiro)) != 0 &&
                testar_k33(&busca, ramos, esquerda)) {
                return 1;
            }
        }
    }
    return 0;
}

int eh_planar_heuristica(GrafoLista *g)
{
    int euler = eh_planar_euler(g);
    if (euler <= 0) {
        return euler;
    }
    if (g->n > LIMITE_KURATOWSKI) {
        return 1;
    }
    int k5 = contem_subdivisao_k5(g);
    int k33 = contem_subdivisao_k33(g);
    if (k5 < 0 || k33 < 0) {
        return -1;
    }
    return !k5 && !k33;
}
