#ifndef HAMILTONIANO_H
#define HAMILTONIANO_H

/* Matriz de adjacencia plana n x n, por linhas. Valor nao zero indica aresta.
 * caminho tem n posicoes. Retorno: 1 se encontrou, 0 se nao existe, -1 por erro.
 */
int encontrar_caminho_hamiltoniano_bt(int n, const int *adj, int *caminho);

/* ciclo tem n + 1 posicoes, com a origem repetida no fim se houver solucao.
 * Em um grafo simples nao orientado, um ciclo precisa de pelo menos 3 vertices.
 */
int encontrar_ciclo_hamiltoniano_bt(int n, const int *adj, int *ciclo);

#endif
