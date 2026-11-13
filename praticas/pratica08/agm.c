#include "agm.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

DSU *criar_dsu(int n)
{
    if (n <= 0) {
        return NULL;
    }
    DSU *d = malloc(sizeof(*d));
    if (d == NULL) {
        return NULL;
    }
    d->pai = calloc((size_t)n, sizeof(*d->pai));
    d->rank = calloc((size_t)n, sizeof(*d->rank));
    if (d->pai == NULL || d->rank == NULL) {
        liberar_dsu(d);
        return NULL;
    }
    d->n = n;
    for (int v = 0; v < n; ++v) {
        d->pai[v] = v;
    }
    return d;
}

void liberar_dsu(DSU *d)
{
    if (d != NULL) {
        free(d->pai);
        free(d->rank);
        free(d);
    }
}

int find(DSU *d, int x)
{
    if (d == NULL || x < 0 || x >= d->n) {
        return -1;
    }
    int raiz = x;
    while (d->pai[raiz] != raiz) {
        raiz = d->pai[raiz];
    }
    /* Compressao completa do caminho, sem chamadas recursivas. */
    while (x != raiz) {
        int proximo = d->pai[x];
        d->pai[x] = raiz;
        x = proximo;
    }
    return raiz;
}

int unir(DSU *d, int x, int y)
{
    int raiz_x = find(d, x);
    int raiz_y = find(d, y);
    if (raiz_x == -1 || raiz_y == -1) {
        return -1;
    }
    if (raiz_x == raiz_y) {
        return 0;
    }
    /* A raiz de menor rank passa a apontar para a de maior rank. */
    if (d->rank[raiz_x] < d->rank[raiz_y]) {
        d->pai[raiz_x] = raiz_y;
    } else {
        d->pai[raiz_y] = raiz_x;
        if (d->rank[raiz_x] == d->rank[raiz_y]) {
            ++d->rank[raiz_x];
        }
    }
    return 1;
}

static int argumentos_validos(int n, int m, const Aresta *arestas,
                              Aresta *agm, long long *custo)
{
    if (n <= 0 || m < 0 || (m > 0 && arestas == NULL) ||
        (n > 1 && agm == NULL) || custo == NULL) {
        return 0;
    }
    for (int i = 0; i < m; ++i) {
        if (arestas[i].u < 0 || arestas[i].u >= n ||
            arestas[i].v < 0 || arestas[i].v >= n) {
            return 0;
        }
    }
    return 1;
}

static int comparar_arestas(const void *a, const void *b)
{
    const Aresta *x = a;
    const Aresta *y = b;
    /* Comparacoes em vez de subtracao evitam overflow com pesos extremos. */
    if (x->peso != y->peso) {
        return (x->peso > y->peso) - (x->peso < y->peso);
    }
    if (x->u != y->u) {
        return (x->u > y->u) - (x->u < y->u);
    }
    return (x->v > y->v) - (x->v < y->v);
}

int kruskal(int n, int m, const Aresta *arestas, Aresta *agm, long long *custo)
{
    if (!argumentos_validos(n, m, arestas, agm, custo)) {
        return -1;
    }
    DSU *d = criar_dsu(n);
    Aresta *ordenadas = m > 0 ? calloc((size_t)m, sizeof(*ordenadas)) : NULL;
    if (d == NULL || (m > 0 && ordenadas == NULL)) {
        liberar_dsu(d);
        free(ordenadas);
        return -1;
    }
    if (m > 0) {
        memcpy(ordenadas, arestas, (size_t)m * sizeof(*ordenadas));
        qsort(ordenadas, (size_t)m, sizeof(*ordenadas), comparar_arestas);
    }

    int quantidade = 0;
    *custo = 0;
    for (int i = 0; i < m && quantidade < n - 1; ++i) {
        Aresta a = ordenadas[i];
        if (unir(d, a.u, a.v) == 1) {
            agm[quantidade++] = a;
            *custo += a.peso;
        }
    }
    free(ordenadas);
    liberar_dsu(d);
    return quantidade;
}

/* Prim usa listas de adjacencia e um heap indexado: posicao[v] permite
 * atualizar a chave do vertice em O(log V), sem procurar no vetor do heap.
 */
typedef struct {
    int vertice, peso;
    size_t proximo;
} Arco;

typedef struct {
    int vertice;
    long long chave; /* LLONG_MAX e infinito; todo peso int e representavel. */
} NoHeap;

typedef struct {
    NoHeap *dados;
    int *posicao;
    int tamanho;
} MinHeap;

static void trocar(MinHeap *heap, int a, int b)
{
    NoHeap auxiliar = heap->dados[a];
    heap->dados[a] = heap->dados[b];
    heap->dados[b] = auxiliar;
    heap->posicao[heap->dados[a].vertice] = a;
    heap->posicao[heap->dados[b].vertice] = b;
}

static void diminuir_chave(MinHeap *heap, int vertice, long long chave)
{
    int indice = heap->posicao[vertice];
    heap->dados[indice].chave = chave;
    while (indice > 0) {
        int pai = (indice - 1) / 2;
        if (heap->dados[pai].chave <= heap->dados[indice].chave) {
            break;
        }
        trocar(heap, pai, indice);
        indice = pai;
    }
}

static NoHeap extrair_minimo(MinHeap *heap)
{
    NoHeap minimo = heap->dados[0];
    heap->posicao[minimo.vertice] = -1;
    --heap->tamanho;
    if (heap->tamanho == 0) {
        return minimo;
    }
    heap->dados[0] = heap->dados[heap->tamanho];
    heap->posicao[heap->dados[0].vertice] = 0;
    int indice = 0;
    while (indice < heap->tamanho / 2) {
        int esquerdo = 2 * indice + 1;
        int direito = esquerdo + 1;
        int menor = esquerdo;
        if (direito < heap->tamanho &&
            heap->dados[direito].chave < heap->dados[esquerdo].chave) {
            menor = direito;
        }
        if (heap->dados[indice].chave <= heap->dados[menor].chave) {
            break;
        }
        trocar(heap, indice, menor);
        indice = menor;
    }
    return minimo;
}

int prim(int n, int m, const Aresta *arestas, int origem,
         Aresta *agm, long long *custo)
{
    if (!argumentos_validos(n, m, arestas, agm, custo) ||
        origem < 0 || origem >= n || (size_t)m > SIZE_MAX / 2 / sizeof(Arco)) {
        return -1;
    }
    size_t *inicio = calloc((size_t)n, sizeof(*inicio));
    int *pai = calloc((size_t)n, sizeof(*pai));
    Arco *arcos = m > 0 ? calloc((size_t)m * 2, sizeof(*arcos)) : NULL;
    MinHeap heap = {NULL, NULL, n};
    heap.dados = calloc((size_t)n, sizeof(*heap.dados));
    heap.posicao = calloc((size_t)n, sizeof(*heap.posicao));
    if (inicio == NULL || pai == NULL || (m > 0 && arcos == NULL) ||
        heap.dados == NULL || heap.posicao == NULL) {
        free(inicio);
        free(pai);
        free(arcos);
        free(heap.dados);
        free(heap.posicao);
        return -1;
    }

    for (int v = 0; v < n; ++v) {
        inicio[v] = SIZE_MAX;
        pai[v] = -1;
        heap.dados[v] = (NoHeap){v, LLONG_MAX};
        heap.posicao[v] = v;
    }
    size_t indice = 0;
    for (int i = 0; i < m; ++i) {
        int u = arestas[i].u;
        int v = arestas[i].v;
        arcos[indice] = (Arco){v, arestas[i].peso, inicio[u]};
        inicio[u] = indice++;
        arcos[indice] = (Arco){u, arestas[i].peso, inicio[v]};
        inicio[v] = indice++;
    }

    diminuir_chave(&heap, origem, 0);
    int quantidade = 0;
    *custo = 0;
    while (heap.tamanho > 0) {
        NoHeap atual = extrair_minimo(&heap);
        int u = atual.vertice;
        if (pai[u] != -1) {
            agm[quantidade++] = (Aresta){pai[u], u, (int)atual.chave};
            *custo += atual.chave;
        }
        /* Sem pai, inicia uma nova componente (inclusive vertices isolados). */
        for (size_t i = inicio[u]; i != SIZE_MAX; i = arcos[i].proximo) {
            int v = arcos[i].vertice;
            int posicao = heap.posicao[v];
            if (posicao != -1 && arcos[i].peso < heap.dados[posicao].chave) {
                pai[v] = u;
                diminuir_chave(&heap, v, arcos[i].peso);
            }
        }
    }
    free(inicio);
    free(pai);
    free(arcos);
    free(heap.dados);
    free(heap.posicao);
    return quantidade;
}
