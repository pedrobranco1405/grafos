#ifndef GRAFO_LISTA_H
#define GRAFO_LISTA_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

/* Grafo simples, nao orientado, com vertices de 0 a n - 1. */
/* Retorna NULL se n <= 0 ou se a alocacao falhar. */
GrafoLista *criar_grafo_lista(int n);

/* Retorno: 1 = alterou; 0 = ja estava assim; -1 = entrada invalida
   ou falha de alocacao. Lacos sao invalidos e duplicatas sao ignoradas. */
int inserir_aresta_lista(GrafoLista *grafo, int u, int v);
int remover_aresta_lista(GrafoLista *grafo, int u, int v);

/* Retorna -1 para grafo ou vertice invalido. */
int grau_lista(const GrafoLista *grafo, int v);
/* Retorna 1 se ha aresta; 0 se nao ha ou se a entrada e invalida. */
int sao_adjacentes_lista(const GrafoLista *grafo, int u, int v);
void liberar_grafo_lista(GrafoLista *grafo);

#endif
