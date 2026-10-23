#include "dijkstra.h"

#include <stdio.h>
#include <stdlib.h>

GrafoListaPonderado *criar_grafo_ponderado(int num_vertices, int orientado)
{
    if (num_vertices <= 0) {
        return NULL;
    }

    GrafoListaPonderado *g = malloc(sizeof(*g));
    if (g == NULL) {
        return NULL;
    }
    g->lista = calloc((size_t)num_vertices, sizeof(*g->lista));
    if (g->lista == NULL) {
        free(g);
        return NULL;
    }
    g->num_vertices = num_vertices;
    g->orientado = orientado != 0;
    return g;
}

int adicionar_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso)
{
    if (g == NULL || u < 0 || u >= g->num_vertices ||
        v < 0 || v >= g->num_vertices || peso < 0) {
        return 0;
    }

    ArestaPonderada *ida = malloc(sizeof(*ida));
    if (ida == NULL) {
        return 0;
    }

    /* Aloca a volta antes de alterar o grafo para evitar uma aresta parcial. */
    ArestaPonderada *volta = NULL;
    if (!g->orientado && u != v) {
        volta = malloc(sizeof(*volta));
        if (volta == NULL) {
            free(ida);
            return 0;
        }
    }

    *ida = (ArestaPonderada){v, peso, g->lista[u]};
    g->lista[u] = ida;
    if (volta != NULL) {
        *volta = (ArestaPonderada){u, peso, g->lista[v]};
        g->lista[v] = volta;
    }
    return 1;
}

void liberar_grafo_ponderado(GrafoListaPonderado *g)
{
    if (g == NULL) {
        return;
    }
    for (int v = 0; v < g->num_vertices; ++v) {
        ArestaPonderada *aresta = g->lista[v];
        while (aresta != NULL) {
            ArestaPonderada *proximo = aresta->proximo;
            free(aresta);
            aresta = proximo;
        }
    }
    free(g->lista);
    free(g);
}

MinHeap *criar_min_heap(int capacidade)
{
    if (capacidade <= 0) {
        return NULL;
    }
    MinHeap *heap = malloc(sizeof(*heap));
    if (heap == NULL) {
        return NULL;
    }
    heap->dados = calloc((size_t)capacidade, sizeof(*heap->dados));
    heap->posicao = calloc((size_t)capacidade, sizeof(*heap->posicao));
    if (heap->dados == NULL || heap->posicao == NULL) {
        liberar_min_heap(heap);
        return NULL;
    }
    heap->capacidade = capacidade;
    heap->tamanho = 0;
    for (int v = 0; v < capacidade; ++v) {
        heap->posicao[v] = -1;
    }
    return heap;
}

static void trocar(MinHeap *heap, int a, int b)
{
    NoHeap auxiliar = heap->dados[a];
    heap->dados[a] = heap->dados[b];
    heap->dados[b] = auxiliar;
    heap->posicao[heap->dados[a].vertice] = a;
    heap->posicao[heap->dados[b].vertice] = b;
}

static void subir(MinHeap *heap, int indice)
{
    while (indice > 0) {
        int pai = (indice - 1) / 2;
        if (heap->dados[pai].distancia <= heap->dados[indice].distancia) {
            break;
        }
        trocar(heap, pai, indice);
        indice = pai;
    }
}

int inserir_min_heap(MinHeap *heap, int vertice, int distancia)
{
    if (heap == NULL || vertice < 0 || vertice >= heap->capacidade ||
        heap->tamanho == heap->capacidade || heap->posicao[vertice] != -1) {
        return 0;
    }
    int indice = heap->tamanho++;
    heap->dados[indice] = (NoHeap){vertice, distancia};
    heap->posicao[vertice] = indice;
    subir(heap, indice);
    return 1;
}

NoHeap extrair_minimo(MinHeap *heap)
{
    if (heap == NULL || heap->tamanho == 0) {
        return (NoHeap){-1, INFINITO};
    }
    NoHeap minimo = heap->dados[0];
    heap->posicao[minimo.vertice] = -1;
    --heap->tamanho;
    if (heap->tamanho == 0) {
        return minimo;
    }

    heap->dados[0] = heap->dados[heap->tamanho];
    heap->posicao[heap->dados[0].vertice] = 0;
    int indice = 0;
    /* Apenas indices com filhos entram no laco. */
    while (indice < heap->tamanho / 2) {
        int esquerdo = 2 * indice + 1;
        int direito = esquerdo + 1;
        int menor = esquerdo;
        if (direito < heap->tamanho &&
            heap->dados[direito].distancia < heap->dados[esquerdo].distancia) {
            menor = direito;
        }
        if (heap->dados[indice].distancia <= heap->dados[menor].distancia) {
            break;
        }
        trocar(heap, indice, menor);
        indice = menor;
    }
    return minimo;
}

int diminuir_chave(MinHeap *heap, int vertice, int distancia)
{
    if (heap == NULL || vertice < 0 || vertice >= heap->capacidade) {
        return 0;
    }
    int indice = heap->posicao[vertice];
    if (indice == -1 || distancia > heap->dados[indice].distancia) {
        return 0;
    }
    heap->dados[indice].distancia = distancia;
    subir(heap, indice);
    return 1;
}

void liberar_min_heap(MinHeap *heap)
{
    if (heap != NULL) {
        free(heap->dados);
        free(heap->posicao);
        free(heap);
    }
}

int dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred)
{
    if (g == NULL || g->lista == NULL || origem < 0 ||
        origem >= g->num_vertices || dist == NULL || pred == NULL) {
        return 0;
    }
    for (int v = 0; v < g->num_vertices; ++v) {
        dist[v] = INFINITO;
        pred[v] = -1;
    }
    /* Verifica inclusive arestas de componentes inacessiveis da origem. */
    for (int v = 0; v < g->num_vertices; ++v) {
        for (ArestaPonderada *a = g->lista[v]; a != NULL; a = a->proximo) {
            if (a->peso < 0 || a->vertice < 0 || a->vertice >= g->num_vertices) {
                return 0;
            }
        }
    }

    MinHeap *heap = criar_min_heap(g->num_vertices);
    if (heap == NULL) {
        return 0;
    }
    dist[origem] = 0;
    for (int v = 0; v < g->num_vertices; ++v) {
        inserir_min_heap(heap, v, dist[v]);
    }

    while (heap->tamanho > 0) {
        NoHeap atual = extrair_minimo(heap);
        int u = atual.vertice;
        if (dist[u] == INFINITO) {
            break; /* Os vertices restantes nao sao alcancaveis. */
        }
        for (ArestaPonderada *a = g->lista[u]; a != NULL; a = a->proximo) {
            int v = a->vertice;
            /* Evita overflow de int e nao reabre vertices finalizados. */
            if (heap->posicao[v] != -1 && a->peso <= INFINITO - dist[u]) {
                int candidato = dist[u] + a->peso;
                if (candidato < dist[v]) {
                    dist[v] = candidato;
                    pred[v] = u;
                    diminuir_chave(heap, v, candidato);
                }
            }
        }
    }
    liberar_min_heap(heap);
    return 1;
}

void reconstruir_caminho(int origem, int destino, int *pred)
{
    if (pred == NULL || origem < 0 || destino < 0) {
        puts("Parametros invalidos para reconstruir o caminho.");
        return;
    }
    size_t tamanho = 1;
    int v = destino;
    while (v != origem && v != -1) {
        v = pred[v];
        ++tamanho;
    }
    if (v == -1) {
        printf("Sem caminho de %d para %d.\n", origem, destino);
        return;
    }

    /* Usa memoria dinamica em vez de recursao para caminhos longos. */
    int *caminho = calloc(tamanho, sizeof(*caminho));
    if (caminho == NULL) {
        puts("Memoria insuficiente para reconstruir o caminho.");
        return;
    }
    v = destino;
    for (size_t i = tamanho; i > 0; --i) {
        caminho[i - 1] = v;
        if (i > 1) {
            v = pred[v];
        }
    }
    for (size_t i = 0; i < tamanho; ++i) {
        printf("%s%d", i == 0 ? "" : " -> ", caminho[i]);
    }
    putchar('\n');
    free(caminho);
}
