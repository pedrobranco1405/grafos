#include <stdio.h>
#include <stdlib.h>
#include "busca_largura.h"
#include "busca_profundidade.h"

enum { VERTICES = 7, MAX_ARESTAS = 6 };

static int demonstrar(const char *nome, const int arestas[][2], int quantidade)
{
    No *adj[VERTICES] = {0};
    No nos[2 * MAX_ARESTAS];
    GrafoLista grafo = {VERTICES, adj};

    /* Os nos dos exemplos ficam neste array durante toda a demonstracao. */
    for (int i = 0; i < quantidade; i++) {
        int u = arestas[i][0];
        int v = arestas[i][1];
        nos[2 * i] = (No){v, adj[u]};
        adj[u] = &nos[2 * i];
        nos[2 * i + 1] = (No){u, adj[v]};
        adj[v] = &nos[2 * i + 1];
    }

    printf("\n=== %s ===\n", nome);
    for (int u = 0; u < grafo.n; u++) {
        printf("%d:", u);
        for (const No *no = adj[u]; no != NULL; no = no->prox) {
            printf(" -> %d", no->destino);
        }
        printf(" -> NULL\n");
    }

    int dist[VERTICES], pred[VERTICES];
    if (!bfs(&grafo, 0, dist, pred)) {
        return 0;
    }
    printf("\nBFS a partir de 0 (-1 indica inalcancavel ou sem predecessor):\n");
    printf("Vertice | Distancia | Predecessor\n");
    for (int v = 0; v < grafo.n; v++) {
        printf("%7d | %9d | %11d\n", v, dist[v], pred[v]);
    }

    int visitado[VERTICES] = {0};
    int entrada[VERTICES], saida[VERTICES];
    int tempo = 0;
    for (int v = 0; v < grafo.n; v++) {
        entrada[v] = -1;
        saida[v] = -1;
    }
    /* Reinicia a busca em cada componente ainda nao visitado. */
    for (int u = 0; u < grafo.n; u++) {
        if (!visitado[u]) {
            dfs_recursiva(&grafo, u, visitado, entrada, saida, &tempo);
        }
    }
    printf("\nDFS recursiva em todos os componentes:\n");
    printf("Vertice | Entrada | Saida\n");
    for (int v = 0; v < grafo.n; v++) {
        printf("%7d | %7d | %5d\n", v, entrada[v], saida[v]);
    }

    int visitado_iterativo[VERTICES] = {0};
    if (!dfs_iterativa(&grafo, 0, visitado_iterativo)) {
        return 0;
    }
    printf("\nVertices alcancaveis de 0 pela DFS com pilha:");
    for (int v = 0; v < grafo.n; v++) {
        if (visitado_iterativo[v]) {
            printf(" %d", v);
        }
    }
    printf("\n");

    int componentes = contar_componentes(&grafo);
    int ciclo = tem_ciclo(&grafo);
    int bipartido = eh_bipartido(&grafo);
    if (componentes < 0 || ciclo < 0 || bipartido < 0) {
        return 0;
    }
    printf("Componentes conexos: %d\n", componentes);
    printf("Tem ciclo: %s\n", ciclo ? "sim" : "nao");
    printf("Eh bipartido: %s\n", bipartido ? "sim" : "nao");
    return 1;
}

int main(void)
{
    const int arvore[][2] = {{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {2, 6}};
    const int ciclo_par[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}};
    const int ciclo_impar[][2] = {{0, 1}, {1, 2}, {3, 4}, {4, 5}, {5, 3}};

    printf("Pratica 02 - BFS e DFS em grafos nao orientados\n");
    if (!demonstrar("Arvore: conexa, sem ciclo e bipartida", arvore, 6) ||
        !demonstrar("Ciclo par, aresta separada e vertice isolado", ciclo_par, 5) ||
        !demonstrar("Ciclo impar fora do componente de 0", ciclo_impar, 5)) {
        fprintf(stderr, "Falha ao executar as buscas.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
