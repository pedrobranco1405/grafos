#include <stdlib.h>
#include "dag.h"

static int grafo_valido(const GrafoLista *g)
{
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL)) {
        return 0;
    }
    for (int u = 0; u < g->n; u++) {
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            if (no->destino < 0 || no->destino >= g->n) {
                return 0;
            }
        }
    }
    return 1;
}

int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho)
{
    if (tamanho == NULL) {
        return NULL;
    }
    *tamanho = 0;
    if (!grafo_valido(g)) {
        return NULL;
    }

    size_t capacidade = g->n > 0 ? (size_t)g->n : 1;
    int *ordem = calloc(capacidade, sizeof *ordem);
    if (ordem == NULL || g->n == 0) {
        return ordem;
    }
    size_t *grau_entrada = calloc((size_t)g->n, sizeof *grau_entrada);
    if (grau_entrada == NULL) {
        free(ordem);
        return NULL;
    }

    for (int u = 0; u < g->n; u++) {
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            grau_entrada[no->destino]++;
        }
    }

    /* O vetor de resultado tambem funciona como fila FIFO: os vertices
       entram no fim e sao processados a partir do inicio. */
    int inicio = 0, fim = 0;
    for (int u = 0; u < g->n; u++) {
        if (grau_entrada[u] == 0) {
            ordem[fim++] = u;
        }
    }
    while (inicio < fim) {
        int u = ordem[inicio++];
        for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
            int v = no->destino;
            grau_entrada[v]--;
            if (grau_entrada[v] == 0) {
                ordem[fim++] = v;
            }
        }
    }
    free(grau_entrada);

    if (fim != g->n) {
        /* Restaram vertices presos em um ciclo: descarta a ordem parcial. */
        free(ordem);
        return NULL;
    }
    *tamanho = fim;
    return ordem;
}

enum { BRANCO, CINZA, PRETO };

static int visitar_dfs(GrafoLista *g, int u, int *cor,
                       int *ordem, int *posicao)
{
    cor[u] = CINZA;
    for (const No *no = g->adj[u]; no != NULL; no = no->prox) {
        int v = no->destino;
        if (cor[v] == CINZA) {
            /* Arco para um vertice na pilha de recursao fecha um ciclo. */
            return 0;
        }
        if (cor[v] == BRANCO && !visitar_dfs(g, v, cor, ordem, posicao)) {
            return 0;
        }
    }
    cor[u] = PRETO;
    if (ordem != NULL) {
        /* Empilha na saida, preenchendo de tras para frente para obter
           diretamente a ordem inversa dos tempos de termino. */
        ordem[--(*posicao)] = u;
    }
    return 1;
}

/* Recebe um grafo ja validado; ordem pode ser NULL para apenas testar DAG. */
static int executar_dfs(GrafoLista *g, int *ordem)
{
    if (g->n == 0) {
        return 1;
    }
    int *cor = calloc((size_t)g->n, sizeof *cor);
    if (cor == NULL) {
        return -1;
    }
    int posicao = g->n;
    for (int u = 0; u < g->n; u++) {
        if (cor[u] == BRANCO && !visitar_dfs(g, u, cor, ordem, &posicao)) {
            free(cor);
            return 0;
        }
    }
    free(cor);
    return 1;
}

int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho)
{
    if (tamanho == NULL) {
        return NULL;
    }
    *tamanho = 0;
    if (!grafo_valido(g)) {
        return NULL;
    }
    size_t capacidade = g->n > 0 ? (size_t)g->n : 1;
    int *ordem = calloc(capacidade, sizeof *ordem);
    if (ordem == NULL) {
        return NULL;
    }
    if (executar_dfs(g, ordem) != 1) {
        free(ordem);
        return NULL;
    }
    *tamanho = g->n;
    return ordem;
}

int eh_dag(GrafoLista *g)
{
    if (!grafo_valido(g)) {
        return -1;
    }
    return executar_dfs(g, NULL);
}
