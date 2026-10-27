#include "bellman_ford.h"

#include <stdio.h>
#include <stdlib.h>

GrafoListaPonderado *criar_grafo_ponderado(int num_vertices, int orientado)
{
    if (num_vertices <= 0) {
        return NULL;
    }
    GrafoListaPonderado *g = malloc(sizeof(*g));
    if (g == NULL) {
        return NULL;
    }
    g->lista = calloc((size_t)num_vertices, sizeof(*g->lista));
    if (g->lista == NULL) {
        free(g);
        return NULL;
    }
    g->num_vertices = num_vertices;
    g->orientado = orientado != 0;
    g->vertice_ciclo = -1;
    return g;
}

int adicionar_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso)
{
    if (g == NULL || u < 0 || u >= g->num_vertices ||
        v < 0 || v >= g->num_vertices) {
        return 0;
    }
    ArestaPonderada *ida = malloc(sizeof(*ida));
    if (ida == NULL) {
        return 0;
    }
    ArestaPonderada *volta = NULL;
    if (!g->orientado && u != v) {
        volta = malloc(sizeof(*volta));
        if (volta == NULL) {
            free(ida);
            return 0;
        }
    }
    *ida = (ArestaPonderada){v, peso, g->lista[u]};
    g->lista[u] = ida;
    if (volta != NULL) {
        *volta = (ArestaPonderada){u, peso, g->lista[v]};
        g->lista[v] = volta;
    }
    g->vertice_ciclo = -1;
    return 1;
}

void liberar_grafo_ponderado(GrafoListaPonderado *g)
{
    if (g == NULL) {
        return;
    }
    for (int v = 0; v < g->num_vertices; ++v) {
        ArestaPonderada *a = g->lista[v];
        while (a != NULL) {
            ArestaPonderada *proximo = a->proximo;
            free(a);
            a = proximo;
        }
    }
    free(g->lista);
    free(g);
}

/* Retorna o ultimo vertice alterado, -1 sem alteracoes, ou -2 se a soma
 * exceder long long. O calculo ampliado evita overflow nas distancias int.
 */
static int relaxar_arestas(GrafoListaPonderado *g, long long *dist, int *pred)
{
    int ultimo = -1;
    for (int u = 0; u < g->num_vertices; ++u) {
        if (dist[u] == LLONG_MAX) {
            continue; /* Um componente inacessivel nao afeta esta origem. */
        }
        for (ArestaPonderada *a = g->lista[u]; a != NULL; a = a->proximo) {
            if ((a->peso > 0 && dist[u] >= LLONG_MAX - a->peso) ||
                (a->peso < 0 && dist[u] < LLONG_MIN - a->peso)) {
                return -2;
            }
            long long candidato = dist[u] + a->peso;
            if (candidato < dist[a->vertice]) {
                dist[a->vertice] = candidato;
                pred[a->vertice] = u;
                ultimo = a->vertice;
            }
        }
    }
    return ultimo;
}

int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred)
{
    if (g == NULL) {
        return -1;
    }
    g->vertice_ciclo = -1;
    if (g->lista == NULL || origem < 0 || origem >= g->num_vertices ||
        dist == NULL || pred == NULL) {
        return -1;
    }
    for (int v = 0; v < g->num_vertices; ++v) {
        dist[v] = INFINITO;
        pred[v] = -1;
        for (ArestaPonderada *a = g->lista[v]; a != NULL; a = a->proximo) {
            if (a->vertice < 0 || a->vertice >= g->num_vertices) {
                return -1;
            }
        }
    }

    long long *custos = calloc((size_t)g->num_vertices, sizeof(*custos));
    if (custos == NULL) {
        return -1;
    }
    for (int v = 0; v < g->num_vertices; ++v) {
        custos[v] = v == origem ? 0 : LLONG_MAX;
    }

    /* Sem ciclos negativos, um caminho minimo tem no maximo n - 1 arestas. */
    for (int passagem = 1; passagem < g->num_vertices; ++passagem) {
        int ultimo = relaxar_arestas(g, custos, pred);
        if (ultimo == -2) {
            free(custos);
            return -1;
        }
        if (ultimo == -1) {
            break; /* Nenhuma melhoria: as distancias ja convergiram. */
        }
    }

    /* Uma melhoria na n-esima passagem prova um ciclo negativo alcancavel.
     * Atualiza pred tambem nesta passagem para permitir reconstruir o ciclo.
     */
    int ultimo = relaxar_arestas(g, custos, pred);
    if (ultimo == -2) {
        free(custos);
        return -1;
    }
    if (ultimo != -1) {
        g->vertice_ciclo = ultimo;
        free(custos);
        return 0;
    }

    for (int v = 0; v < g->num_vertices; ++v) {
        if (custos[v] != LLONG_MAX &&
            (custos[v] < INT_MIN || custos[v] >= INFINITO)) {
            free(custos);
            return -1;
        }
    }
    for (int v = 0; v < g->num_vertices; ++v) {
        dist[v] = custos[v] == LLONG_MAX ? INFINITO : (int)custos[v];
    }
    free(custos);
    return 1;
}

int reconstruir_ciclo_negativo(int n, int vertice, int *pred)
{
    if (n <= 0 || pred == NULL || vertice < 0 || vertice >= n) {
        return 0;
    }
    /* A testemunha pode estar depois do ciclo: n passos garantem a entrada. */
    for (int i = 0; i < n; ++i) {
        vertice = pred[vertice];
        if (vertice < 0 || vertice >= n) {
            return 0;
        }
    }

    int *ciclo = calloc((size_t)n, sizeof(*ciclo));
    if (ciclo == NULL) {
        return 0;
    }
    int tamanho = 0;
    int atual = vertice;
    do {
        if (atual < 0 || atual >= n || tamanho == n) {
            free(ciclo);
            return 0;
        }
        ciclo[tamanho++] = atual;
        atual = pred[atual];
    } while (atual != vertice);

    /* pred anda contra as arestas; inverte a ordem e fecha o ciclo. */
    printf("Ciclo negativo: %d", vertice);
    for (int i = tamanho - 1; i >= 0; --i) {
        printf(" -> %d", ciclo[i]);
    }
    putchar('\n');
    free(ciclo);
    return 1;
}
