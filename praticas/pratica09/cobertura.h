#ifndef COBERTURA_H
#define COBERTURA_H

typedef struct {
    int u, v;
} Aresta;

/* Grafo nao orientado: n > 0 vertices, numerados de 0 a n - 1, e m arestas.
 * Lacos e arestas paralelas sao permitidos; arestas pode ser NULL se m == 0.
 * selecionados deve ter n posicoes. Os algoritmos preenchem com 0 ou 1 e
 * retornam o tamanho da cobertura, ou -1 para dados invalidos/falta de memoria.
 * A entrada de arestas nao e modificada.
 */
int cobertura_minima_bt(int n, int m, const Aresta *arestas, int *selecionados);
int cobertura_gulosa(int n, int m, const Aresta *arestas, int *selecionados);

/* Retorna 1 se todas as arestas tem ao menos uma ponta selecionada,
 * 0 se alguma nao esta coberta, ou -1 para dados invalidos (inclusive
 * valores diferentes de 0 e 1 em selecionados).
 */
int eh_cobertura(int n, int m, const Aresta *arestas, const int *selecionados);

#endif
