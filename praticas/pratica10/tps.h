#ifndef TPS_H
#define TPS_H

/* Matrizes planas n x n, por linhas, em buffers distintos.
 * Peso 1 para aresta, 2 para ausencia e 0 na diagonal.
 * Retorna 1 em sucesso, -1 para argumentos invalidos.
 */
int grafo_para_tsp(int n, const int *adj, int *pesos);

/* TSP simetrico, com pesos inteiros nao negativos e diagonal zero.
 * rota deve ter n + 1 posicoes: comeca e termina na origem.
 * Retorno: custo do circuito, ou -1 por argumentos invalidos/falta de memoria.
 */
long long tsp_forca_bruta(int n, const int *pesos, int origem, int *rota);
long long tsp_vizinho_mais_proximo(int n, const int *pesos, int origem, int *rota);
long long tsp_insercao(int n, const int *pesos, int origem, int *rota);

#endif
