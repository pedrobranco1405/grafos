#ifndef BUSCA_PROFUNDIDADE_H
#define BUSCA_PROFUNDIDADE_H

#include "busca_largura.h"

typedef struct {
    int *dados;
    int topo, capacidade;
} Pilha;

/* Operacoes retornam 1 no sucesso, 0 em erro, pilha cheia ou vazia.
   Inicialize a pilha antes de usa-la; libere-a antes de reinicializar. */
int inicializar_pilha(Pilha *pilha, int capacidade);
int empilhar(Pilha *pilha, int valor);
int desempilhar(Pilha *pilha, int *valor);
void liberar_pilha(Pilha *pilha);

/* Vetores com n posicoes: visitado inicialmente 0, entrada/saida -1.
   Inicialize *tempo = 0; preserve-o entre componentes da mesma busca.
   Visita apenas vertices novos alcancaveis de u, registrando os tempos. */
void dfs_recursiva(GrafoLista *g, int u, int *visitado,
                   int *entrada, int *saida, int *tempo);

/* visitado tem n posicoes e deve comecar zerado; visitas anteriores
   sao preservadas. Retorna 1 no sucesso, 0 em erro. */
int dfs_iterativa(GrafoLista *g, int origem, int *visitado);

/* Retorna a quantidade de componentes, ou -1 em erro. */
int contar_componentes(GrafoLista *g);
/* Grafo simples nao orientado: 1 = tem ciclo, 0 = nao, -1 = erro. */
int tem_ciclo(GrafoLista *g);

#endif
