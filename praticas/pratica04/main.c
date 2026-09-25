#include <stdio.h>
#include <stdlib.h>
#include "conectividade.h"
#include "planaridade.h"

enum { MAX_VERTICES = 8, MAX_ARESTAS = 12 };

static int demonstrar(const char *nome, int n, const int arestas[][2],
                      int quantidade)
{
    No *adj[MAX_VERTICES] = {0};
    No nos[2 * MAX_ARESTAS];
    GrafoLista grafo = {n, adj};
    for (int i = 0; i < quantidade; i++) {
        int u = arestas[i][0];
        int v = arestas[i][1];
        nos[2 * i] = (No){v, adj[u]};
        adj[u] = &nos[2 * i];
        nos[2 * i + 1] = (No){u, adj[v]};
        adj[v] = &nos[2 * i + 1];
    }

    int articulacao[MAX_VERTICES] = {0};
    int quantidade_articulacoes = detectar_articulacoes(&grafo, articulacao);
    int quantidade_pontes;
    Ponte *pontes = detectar_pontes(&grafo, &quantidade_pontes);
    int euler = eh_planar_euler(&grafo);
    int k5 = contem_subdivisao_k5(&grafo);
    int k33 = contem_subdivisao_k33(&grafo);
    int planar = eh_planar_heuristica(&grafo);
    if (quantidade_articulacoes < 0 || pontes == NULL || euler < 0 ||
        k5 < 0 || k33 < 0 || planar < 0) {
        free(pontes);
        return 0;
    }

    printf("\n=== %s ===\n", nome);
    printf("Vertices de corte (%d):", quantidade_articulacoes);
    if (quantidade_articulacoes == 0) {
        printf(" nenhum");
    }
    for (int u = 0; u < n; u++) {
        if (articulacao[u]) {
            printf(" %d", u);
        }
    }
    printf("\nPontes (%d):", quantidade_pontes);
    if (quantidade_pontes == 0) {
        printf(" nenhuma");
    }
    for (int i = 0; i < quantidade_pontes; i++) {
        printf(" (%d,%d)", pontes[i].u, pontes[i].v);
    }
    printf("\nPassa na condicao de Euler: %s\n", euler ? "sim" : "nao");
    printf("Contem subdivisao de K5: %s\n", k5 ? "sim" : "nao");
    printf("Contem subdivisao de K3,3: %s\n", k33 ? "sim" : "nao");
    printf("Resultado de planaridade: %s\n", planar ? "planar" : "nao planar");
    free(pontes);
    return 1;
}

int main(void)
{
    const int duas_triangulos[][2] = {
        {0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 5}, {5, 3}
    };
    const int k5[][2] = {
        {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 2},
        {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}
    };
    const int k33[][2] = {
        {0, 3}, {0, 4}, {0, 5}, {1, 3}, {1, 4},
        {1, 5}, {2, 3}, {2, 4}, {2, 5}
    };
    const int k33_subdividido[][2] = {
        {0, 6}, {6, 3}, {0, 4}, {0, 5}, {1, 3},
        {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}
    };

    printf("Pratica 04 - Conectividade e planaridade\n");
    if (!demonstrar("Dois triangulos ligados por uma ponte", 6,
                    duas_triangulos, 7) ||
        !demonstrar("K5", 5, k5, 10) ||
        !demonstrar("K3,3", 6, k33, 9) ||
        !demonstrar("Subdivisao de K3,3", 7, k33_subdividido, 10)) {
        fprintf(stderr, "Falha ao analisar um dos grafos.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
