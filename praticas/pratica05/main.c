#include <stdio.h>
#include <stdlib.h>
#include "coloracao.h"

enum { MAX_VERTICES = 7, MAX_ARESTAS = 12 };

static void exibir_coloracao(const char *nome, const int *cores,
                             int n, int num_cores)
{
    printf("%s (%d cores):", nome, num_cores);
    if (n == 0) {
        printf(" grafo vazio");
    }
    for (int u = 0; u < n; u++) {
        printf(" v%d=cor%d", u, cores[u]);
    }
    printf("\n");
}

static int demonstrar(const char *nome, int n, const int arestas[][2],
                      int quantidade)
{
    No *adj[MAX_VERTICES] = {0};
    No nos[2 * MAX_ARESTAS];
    GrafoLista grafo = {n, n == 0 ? NULL : adj};
    for (int i = 0; i < quantidade; i++) {
        int u = arestas[i][0];
        int v = arestas[i][1];
        nos[2 * i] = (No){v, adj[u]};
        adj[u] = &nos[2 * i];
        nos[2 * i + 1] = (No){u, adj[v]};
        adj[v] = &nos[2 * i + 1];
    }

    int cores_gulosa, cores_welsh;
    int *gulosa = coloracao_gulosa(&grafo, &cores_gulosa);
    int *welsh = coloracao_welsh_powell(&grafo, &cores_welsh);
    int bipartido = eh_bipartido(&grafo);
    if (gulosa == NULL || welsh == NULL || bipartido < 0) {
        free(gulosa);
        free(welsh);
        return 0;
    }

    printf("\n=== %s ===\n", nome);
    exibir_coloracao("Gulosa", gulosa, n, cores_gulosa);
    exibir_coloracao("Welsh-Powell", welsh, n, cores_welsh);
    printf("Eh bipartido: %s\n", bipartido ? "sim" : "nao");
    free(gulosa);
    free(welsh);
    return 1;
}

int main(void)
{
    const int arvore[][2] = {
        {0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {2, 6}
    };
    const int ciclo_impar[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}
    };
    const int k4[][2] = {
        {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}
    };
    const int desconexo[][2] = {
        {0, 1}, {1, 2}, {3, 4}, {4, 5}, {5, 3}
    };

    printf("Pratica 05 - Coloracao de vertices\n");
    if (!demonstrar("Arvore", 7, arvore, 6) ||
        !demonstrar("Ciclo impar C5", 5, ciclo_impar, 5) ||
        !demonstrar("Grafo completo K4", 4, k4, 6) ||
        !demonstrar("Grafo desconexo", 7, desconexo, 5) ||
        !demonstrar("Grafo vazio", 0, NULL, 0)) {
        fprintf(stderr, "Falha ao colorir um dos grafos.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
