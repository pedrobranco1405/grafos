#include "dijkstra.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /* Grafo nao orientado: o vertice 6 fica isolado de proposito. */
    const int arestas[][3] = {
        {0, 1, 7}, {0, 2, 9}, {0, 5, 14}, {1, 2, 10}, {1, 3, 15},
        {2, 3, 11}, {2, 5, 2}, {3, 4, 6}, {4, 5, 9}
    };
    GrafoListaPonderado *g = criar_grafo_ponderado(7, 0);
    if (g == NULL) {
        fputs("Erro ao criar o grafo.\n", stderr);
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < sizeof(arestas) / sizeof(arestas[0]); ++i) {
        if (!adicionar_aresta_ponderada(g, arestas[i][0], arestas[i][1],
                                      arestas[i][2])) {
            fputs("Erro ao adicionar uma aresta.\n", stderr);
            liberar_grafo_ponderado(g);
            return EXIT_FAILURE;
        }
    }

    int dist[7], pred[7];
    int origem = 0;
    if (!dijkstra(g, origem, dist, pred)) {
        fputs("Nao foi possivel executar Dijkstra.\n", stderr);
        liberar_grafo_ponderado(g);
        return EXIT_FAILURE;
    }

    printf("Dijkstra com Min-Heap - origem: %d\n", origem);
    puts("Vertice | Distancia | Predecessor");
    for (int v = 0; v < g->num_vertices; ++v) {
        if (dist[v] == INFINITO) {
            printf("%7d |       INF | %d\n", v, pred[v]);
        } else {
            printf("%7d | %9d | %d\n", v, dist[v], pred[v]);
        }
    }

    puts("\nCaminho minimo de 0 para 4 (custo 20):");
    reconstruir_caminho(origem, 4, pred);
    puts("\nCaminho da origem para ela mesma:");
    reconstruir_caminho(origem, origem, pred);
    puts("\nTentativa de caminho para o vertice isolado:");
    reconstruir_caminho(origem, 6, pred);

    liberar_grafo_ponderado(g);
    return EXIT_SUCCESS;
}
