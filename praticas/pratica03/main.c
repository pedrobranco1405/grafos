#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

enum { MAX_VERTICES = 6, MAX_ARCOS = 6 };

static void exibir_ordem(const char *algoritmo, const int *ordem, int tamanho)
{
    printf("%s:", algoritmo);
    if (ordem == NULL) {
        printf(" existe ciclo; ordenacao impossivel.\n");
        return;
    }
    if (tamanho == 0) {
        printf(" ordem vazia");
    }
    for (int i = 0; i < tamanho; i++) {
        printf(" %d", ordem[i]);
    }
    printf("\n");
}

static int demonstrar(const char *nome, int n, const int arcos[][2], int quantidade)
{
    No *adj[MAX_VERTICES] = {0};
    No nos[MAX_ARCOS];
    GrafoLista grafo = {n, adj};

    /* Os nos de cada exemplo permanecem neste array durante as buscas.
       Insere somente u -> v, pois os grafos desta pratica sao direcionados. */
    for (int i = 0; i < quantidade; i++) {
        int u = arcos[i][0];
        int v = arcos[i][1];
        nos[i] = (No){v, adj[u]};
        adj[u] = &nos[i];
    }

    printf("\n=== %s ===\n", nome);
    for (int u = 0; u < n; u++) {
        printf("%d:", u);
        for (const No *no = adj[u]; no != NULL; no = no->prox) {
            printf(" -> %d", no->destino);
        }
        printf(" -> NULL\n");
    }

    int tamanho_kahn, tamanho_dfs;
    int dag = eh_dag(&grafo);
    int *kahn = ordenacao_topologica_kahn(&grafo, &tamanho_kahn);
    int *dfs = ordenacao_topologica_dfs(&grafo, &tamanho_dfs);
    if (dag < 0 || (dag == 1 && (kahn == NULL || dfs == NULL)) ||
        (dag == 0 && (kahn != NULL || dfs != NULL))) {
        free(kahn);
        free(dfs);
        fprintf(stderr, "Falha ao executar a ordenacao topologica.\n");
        return 0;
    }

    printf("Eh DAG: %s\n", dag ? "sim" : "nao");
    exibir_ordem("Kahn", kahn, tamanho_kahn);
    exibir_ordem("DFS", dfs, tamanho_dfs);
    free(kahn);
    free(dfs);
    return 1;
}

int main(void)
{
    const int dependencias[][2] = {{5, 2}, {5, 0}, {4, 0}, {4, 1}, {2, 3}, {3, 1}};
    const int desconexo[][2] = {{0, 1}, {2, 3}, {3, 4}};
    const int ciclo[][2] = {{0, 1}, {1, 2}, {3, 4}, {4, 5}, {5, 3}};
    const int laco[][2] = {{0, 0}};

    printf("Pratica 03 - Ordenacao topologica em digrafos\n");
    printf("Cada arco u -> v exige que u apareca antes de v.\n");
    if (!demonstrar("DAG de dependencias", 6, dependencias, 6) ||
        !demonstrar("DAG desconexo com vertice isolado", 6, desconexo, 3) ||
        !demonstrar("Ciclo em um componente separado", 6, ciclo, 5) ||
        !demonstrar("Laco em um unico vertice", 1, laco, 1) ||
        !demonstrar("Grafo vazio", 0, NULL, 0)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
