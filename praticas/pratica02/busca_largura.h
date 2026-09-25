#ifndef BUSCA_LARGURA_H
#define BUSCA_LARGURA_H

/* Mesma representacao por lista da pratica 01, sem depender daquela pasta.
   As buscas recebem grafos nao orientados com destinos entre 0 e n - 1. */
typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

typedef struct {
    int *dados;
    int capacidade, inicio, fim, tamanho;
} Fila;

/* Operacoes retornam 1 no sucesso, 0 em erro, fila cheia ou vazia.
   Inicialize a fila antes de usa-la; libere-a antes de reinicializar. */
int inicializar_fila(Fila *fila, int capacidade);
int enfileirar(Fila *fila, int valor);
int desenfileirar(Fila *fila, int *valor);
void liberar_fila(Fila *fila);

/* dist e pred devem ter n posicoes. Inalcancaveis recebem -1;
   a origem recebe dist = 0 e pred = -1. Retorna 1 no sucesso, 0 em erro. */
int bfs(GrafoLista *g, int origem, int *dist, int *pred);

/* Examina todos os componentes: 1 = bipartido, 0 = nao, -1 = erro. */
int eh_bipartido(GrafoLista *g);

#endif
