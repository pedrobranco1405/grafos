#include "bellman_ford.h"

#include <stdio.h>
#include <stdlib.h>

static int executar_exemplo(const char *titulo, int n,
                            const int arestas[][3], size_t quantidade)
{
    GrafoListaPonderado *g = criar_grafo_ponderado(n, 1);
    if (g == NULL) {
        fputs("Erro ao criar o grafo.\n", stderr);
        return 0;
    }
    for (size_t i = 0; i < quantidade; ++i) {
        if (!adicionar_aresta_ponderada(g, arestas[i][0], arestas[i][1],
                                      arestas[i][2])) {
            fputs("Erro ao adicionar uma aresta.\n", stderr);
            liberar_grafo_ponderado(g);
            return 0;
        }
    }
    int *dist = calloc((size_t)n, sizeof(*dist));
    int *pred = calloc((size_t)n, sizeof(*pred));
    if (dist == NULL || pred == NULL) {
        fputs("Erro ao alocar os vetores.\n", stderr);
        free(dist);
        free(pred);
        liberar_grafo_ponderado(g);
        return 0;
    }

    printf("\n%s\nOrigem: 0\n", titulo);
    int resultado = bellman_ford(g, 0, dist, pred);
    int sucesso = 1;
    if (resultado == 1) {
        puts("Sem ciclo negativo alcancavel a partir da origem.");
        puts("Vertice | Distancia | Predecessor");
        for (int v = 0; v < n; ++v) {
            if (dist[v] == INFINITO) {
                printf("%7d |       INF | %d\n", v, pred[v]);
            } else {
                printf("%7d | %9d | %d\n", v, dist[v], pred[v]);
            }
        }
    } else if (resultado == 0) {
        puts("Ciclo negativo alcancavel detectado!");
        sucesso = reconstruir_ciclo_negativo(n, g->vertice_ciclo, pred);
        if (!sucesso) {
            fputs("Erro ao reconstruir o ciclo negativo.\n", stderr);
        }
    } else {
        fputs("Erro: dados invalidos, memoria ou custo fora do intervalo int.\n",
              stderr);
        sucesso = 0;
    }

    free(dist);
    free(pred);
    liberar_grafo_ponderado(g);
    return sucesso;
}

int main(void)
{
    /* Distancias esperadas: 0, 2, 7, 4, -2, INF. */
    const int sem_ciclo[][3] = {
        {0, 1, 6}, {0, 2, 7}, {1, 2, 8}, {1, 3, 5}, {1, 4, -4},
        {2, 3, -3}, {2, 4, 9}, {3, 1, -2}, {4, 0, 2}, {4, 3, 7}
    };
    /* O ciclo 1 -> 2 -> 3 -> 1 tem custo -3. */
    const int com_ciclo[][3] = {
        {0, 1, 1}, {1, 2, -1}, {2, 3, -1}, {3, 1, -1}
    };
    /* O ciclo 2 -> 3 -> 2 e negativo, mas nao e alcancavel da origem 0. */
    const int ciclo_isolado[][3] = {
        {0, 1, 2}, {2, 3, -2}, {3, 2, -1}
    };

    puts("Pratica 07 - Bellman-Ford");
    if (!executar_exemplo("1. Pesos negativos sem ciclo negativo", 6,
                          sem_ciclo, sizeof(sem_ciclo) / sizeof(sem_ciclo[0])) ||
        !executar_exemplo("2. Ciclo negativo alcancavel", 4,
                          com_ciclo, sizeof(com_ciclo) / sizeof(com_ciclo[0])) ||
        !executar_exemplo("3. Ciclo negativo em componente isolado", 4,
                          ciclo_isolado,
                          sizeof(ciclo_isolado) / sizeof(ciclo_isolado[0]))) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
