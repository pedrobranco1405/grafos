#ifndef AGM_H
#define AGM_H

typedef struct {
    int *pai, *rank;
    int n;
} DSU;

typedef struct {
    int u, v, peso;
} Aresta;

DSU *criar_dsu(int n);
void liberar_dsu(DSU *d);
/* Retorna a raiz, ou -1 para um argumento invalido. */
int find(DSU *d, int x);
/* Retorna 1 se uniu, 0 se ja estavam unidos, ou -1 para argumento invalido. */
int unir(DSU *d, int x, int y);

/* Grafo nao orientado, com n > 0 vertices numerados de 0 a n - 1.
 * arestas contem m arestas; pesos negativos e arestas paralelas sao aceitos.
 * agm deve ter espaco para n - 1 arestas (pode ser NULL quando n == 1).
 * Retorno: quantidade de arestas selecionadas, ou -1 em caso de erro.
 * Se retornar n - 1, o resultado e uma arvore geradora minima.
 * Se retornar menos, o grafo e desconexo e o resultado e uma floresta
 * geradora minima, com n - retorno componentes. custo recebe a soma dos pesos.
 * O vetor original de arestas nao e alterado.
 */
int kruskal(int n, int m, const Aresta *arestas, Aresta *agm, long long *custo);
int prim(int n, int m, const Aresta *arestas, int origem,
         Aresta *agm, long long *custo);

#endif
