#include <stdio.h>
#include <stdlib.h>
#include "grafo_matriz.h"
#include "grafo_lista.h"

static void exibir_grafos(const GrafoMatriz *matriz, const GrafoLista *lista)
{
    printf("\nMatriz de adjacencia:\n   ");
    for (int v = 0; v < matriz->n; v++) {
        printf("%3d", v);
    }
    printf("\n");
    for (int u = 0; u < matriz->n; u++) {
        printf("%3d", u);
        for (int v = 0; v < matriz->n; v++) {
            printf("%3d", matriz->adj[u][v]);
        }
        printf("\n");
    }

    printf("\nLista de adjacencia:\n");
    for (int u = 0; u < lista->n; u++) {
        printf("%d:", u);
        for (const No *no = lista->adj[u]; no != NULL; no = no->prox) {
            printf(" -> %d", no->destino);
        }
        printf(" -> NULL\n");
    }
}

static void exibir_consultas(const GrafoMatriz *matriz, const GrafoLista *lista)
{
    printf("\nVertice | Grau (matriz) | Grau (lista)\n");
    for (int v = 0; v < matriz->n; v++) {
        printf("%7d | %13d | %12d\n", v, grau_matriz(matriz, v),
               grau_lista(lista, v));
    }

    printf("\n0 e 2 sao adjacentes? Matriz: %s; Lista: %s\n",
           sao_adjacentes_matriz(matriz, 0, 2) ? "sim" : "nao",
           sao_adjacentes_lista(lista, 0, 2) ? "sim" : "nao");
    printf("0 e 4 sao adjacentes? Matriz: %s; Lista: %s\n",
           sao_adjacentes_matriz(matriz, 0, 4) ? "sim" : "nao",
           sao_adjacentes_lista(lista, 0, 4) ? "sim" : "nao");
}

int main(void)
{
    GrafoMatriz *matriz = criar_grafo_matriz(5);
    GrafoLista *lista = criar_grafo_lista(5);
    if (matriz == NULL || lista == NULL) {
        fprintf(stderr, "Erro ao alocar os grafos.\n");
        liberar_grafo_matriz(matriz);
        liberar_grafo_lista(lista);
        return EXIT_FAILURE;
    }

    const int arestas[][2] = {{0, 1}, {0, 2}, {1, 2}, {1, 3}, {3, 4}};
    const size_t quantidade = sizeof arestas / sizeof arestas[0];

    for (size_t i = 0; i < quantidade; i++) {
        int u = arestas[i][0];
        int v = arestas[i][1];
        if (inserir_aresta_matriz(matriz, u, v) != 1 ||
            inserir_aresta_lista(lista, u, v) != 1) {
            fprintf(stderr, "Erro ao inserir a aresta (%d, %d).\n", u, v);
            liberar_grafo_matriz(matriz);
            liberar_grafo_lista(lista);
            return EXIT_FAILURE;
        }
    }

    printf("Pratica 01 - Grafo simples nao orientado\n");
    printf("Arestas: (0,1), (0,2), (1,2), (1,3), (3,4)\n");
    exibir_grafos(matriz, lista);
    exibir_consultas(matriz, lista);

    printf("\nRemovendo a aresta (0, 2)...\n");
    if (remover_aresta_matriz(matriz, 0, 2) != 1 ||
        remover_aresta_lista(lista, 0, 2) != 1) {
        fprintf(stderr, "Erro ao remover a aresta.\n");
        liberar_grafo_matriz(matriz);
        liberar_grafo_lista(lista);
        return EXIT_FAILURE;
    }
    exibir_grafos(matriz, lista);
    exibir_consultas(matriz, lista);

    liberar_grafo_matriz(matriz);
    liberar_grafo_lista(lista);
    return EXIT_SUCCESS;
}
