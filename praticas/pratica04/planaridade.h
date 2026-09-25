#ifndef PLANARIDADE_H
#define PLANARIDADE_H

#include "conectividade.h"

/* Condicao necessaria de Euler para grafos simples: m <= 3n - 6.
   Retorna 1 se passa, 0 se viola e -1 para entrada invalida. */
int eh_planar_euler(GrafoLista *g);

/* Busca exaustiva de subdivisoes (subgrafos homeomorfos).
   Suportada para grafos simples nao orientados com ate 10 vertices.
   Retorna 1 se encontrou, 0 se nao encontrou e -1 em entrada invalida
   ou quando o limite de 10 vertices for excedido. */
int contem_subdivisao_k5(GrafoLista *g);
int contem_subdivisao_k33(GrafoLista *g);

/* Retorna 0 quando Euler ou Kuratowski provam que o grafo nao e planar.
   Para n <= 10, o teste de Kuratowski e exaustivo. Para n > 10,
   retorna 1 apenas quando a condicao necessaria de Euler e satisfeita.
   Retorna -1 em entrada invalida. */
int eh_planar_heuristica(GrafoLista *g);

#endif
