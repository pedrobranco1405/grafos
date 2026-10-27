#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <limits.h>

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
    /* Testemunha da ultima relaxacao na n-esima passagem; -1 sem ciclo. */
    int vertice_ciclo;
} GrafoListaPonderado;

/* Vertices de 0 a num_vertices - 1; pesos negativos sao permitidos. */
GrafoListaPonderado *criar_grafo_ponderado(int num_vertices, int orientado);
int adicionar_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

/* dist e pred devem ter num_vertices posicoes.
 * Retorno: 1 = sucesso; 0 = ciclo negativo alcancavel a partir da origem;
 * -1 = argumento invalido, falha de memoria ou custo nao representavel.
 * No sucesso, dist fica em [INT_MIN, INT_MAX - 1]; INFINITO indica ausencia
 * de caminho, e pred recebe -1 na origem e nos vertices inacessiveis.
 * Em caso de ciclo, dist nao representa caminhos minimos; use pred e
 * g->vertice_ciclo para reconstruir o ciclo negativo.
 */
int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/* Retrocede n vezes em pred para entrar no ciclo e imprime suas arestas
 * na direcao correta. Use o vertice_ciclo e o pred da chamada anterior.
 * Retorna 1 se imprimiu um ciclo, ou 0 se os dados/memoria forem invalidos.
 */
int reconstruir_ciclo_negativo(int n, int vertice, int *pred);

#endif
