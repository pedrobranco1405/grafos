#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <limits.h>

/* INT_MAX representa infinito; custos finitos devem ser menores que ele. */
#define INFINITO INT_MAX

typedef struct ArestaPonderada {
    int vertice;
    int peso;
    struct ArestaPonderada *proximo;
} ArestaPonderada;

typedef struct {
    ArestaPonderada **lista;
    int num_vertices;
    int orientado;
} GrafoListaPonderado;

typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao; /* Mapeia vertice -> indice; -1 indica ausencia. */
} MinHeap;

/* Vertices numerados de 0 a num_vertices - 1. */
GrafoListaPonderado *criar_grafo_ponderado(int num_vertices, int orientado);
int adicionar_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

/* O heap nasce vazio e comporta vertices de 0 a capacidade - 1. */
MinHeap *criar_min_heap(int capacidade);
int inserir_min_heap(MinHeap *heap, int vertice, int distancia);
/* Heap vazio: retorna { -1, INFINITO }. */
NoHeap extrair_minimo(MinHeap *heap);
int diminuir_chave(MinHeap *heap, int vertice, int distancia);
void liberar_min_heap(MinHeap *heap);

/* dist e pred devem ter num_vertices posicoes. Retorna 1 em caso de sucesso.
 * Pesos negativos, argumentos invalidos ou falha de alocacao retornam 0.
 * Custos >= INFINITO nao sao representaveis e permanecem como INFINITO.
 * pred[origem] e predecessores de vertices sem caminho recebem -1.
 */
int dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/* Imprime o caminho usando pred produzido por Dijkstra para a mesma origem.
 * origem e destino devem ser indices validos desse vetor.
 */
void reconstruir_caminho(int origem, int destino, int *pred);

#endif
