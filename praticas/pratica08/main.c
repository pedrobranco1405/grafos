#include "agm.h"

#include <stdio.h>
#include <stdlib.h>

static void mostrar_resultado(const char *algoritmo, int n, const Aresta *agm,
                              int quantidade, long long custo)
{
    printf("\n%s\n", algoritmo);
    if (quantidade == n - 1) {
        puts("Arvore geradora minima:");
    } else {
        printf("Grafo desconexo: floresta geradora minima com %d componentes.\n",
               n - quantidade);
    }
    for (int i = 0; i < quantidade; ++i) {
        printf("%d -- %d (peso %d)\n", agm[i].u, agm[i].v, agm[i].peso);
    }
    printf("Arestas selecionadas: %d\nCusto total: %lld\n", quantidade, custo);
}

static int executar_exemplo(const char *titulo, int n,
                            const Aresta *arestas, int m)
{
    Aresta *resultado_kruskal = calloc((size_t)n, sizeof(*resultado_kruskal));
    Aresta *resultado_prim = calloc((size_t)n, sizeof(*resultado_prim));
    if (resultado_kruskal == NULL || resultado_prim == NULL) {
        fputs("Erro ao alocar os resultados.\n", stderr);
        free(resultado_kruskal);
        free(resultado_prim);
        return 0;
    }
    long long custo_kruskal = 0, custo_prim = 0;
    int qtd_kruskal = kruskal(n, m, arestas, resultado_kruskal, &custo_kruskal);
    int qtd_prim = prim(n, m, arestas, 0, resultado_prim, &custo_prim);
    int sucesso = 1;
    if (qtd_kruskal < 0 || qtd_prim < 0) {
        fputs("Erro: argumentos invalidos ou memoria insuficiente.\n", stderr);
        sucesso = 0;
    } else {
        printf("\n=== %s ===\nVertices: %d; arestas: %d\n", titulo, n, m);
        mostrar_resultado("Kruskal (Union-Find)", n, resultado_kruskal,
                          qtd_kruskal, custo_kruskal);
        mostrar_resultado("Prim (Min-Heap)", n, resultado_prim,
                          qtd_prim, custo_prim);
        if (qtd_kruskal != qtd_prim || custo_kruskal != custo_prim) {
            fputs("Erro: os algoritmos produziram custos diferentes.\n", stderr);
            sucesso = 0;
        }
    }
    free(resultado_kruskal);
    free(resultado_prim);
    return sucesso;
}

int main(void)
{
    /* Grafo classico: custo esperado da AGM = 37. */
    const Aresta conectado[] = {
        {0, 1, 4}, {0, 7, 8}, {1, 2, 8}, {1, 7, 11}, {2, 3, 7},
        {2, 8, 2}, {2, 5, 4}, {3, 4, 9}, {3, 5, 14}, {4, 5, 10},
        {5, 6, 2}, {6, 7, 1}, {6, 8, 6}, {7, 8, 7}
    };
    /* Custo = 1. O laco negativo e ignorado; arestas paralelas sao aceitas. */
    const Aresta pesos_variados[] = {
        {0, 1, -2}, {1, 2, 0}, {0, 2, 3}, {2, 3, 4},
        {1, 3, 2}, {3, 4, 1}, {0, 0, -100}, {0, 1, 5}
    };
    /* Tres componentes, incluindo o vertice 5 isolado; custo da floresta = 2. */
    const Aresta desconectado[] = {
        {0, 1, 3}, {1, 2, 1}, {0, 2, 5}, {3, 4, -2}
    };

    puts("Pratica 08 - Arvore Geradora Minima");
    if (!executar_exemplo("1. Grafo conectado", 9, conectado,
                          (int)(sizeof(conectado) / sizeof(conectado[0]))) ||
        !executar_exemplo("2. Pesos negativos, zero e arestas paralelas", 5,
                          pesos_variados,
                          (int)(sizeof(pesos_variados) / sizeof(pesos_variados[0]))) ||
        !executar_exemplo("3. Grafo desconectado", 6, desconectado,
                          (int)(sizeof(desconectado) / sizeof(desconectado[0])))) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
