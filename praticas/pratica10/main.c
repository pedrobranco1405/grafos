#include "hamiltoniano.h"
#include "tps.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void adicionar_aresta(int n, int *adj, int u, int v)
{
    adj[(size_t)u * n + v] = adj[(size_t)v * n + u] = 1;
}

static void imprimir_rota(const char *rotulo, int tamanho, const int *rota)
{
    printf("%s%d", rotulo, rota[0]);
    for (int i = 1; i < tamanho; ++i) {
        printf(" -> %d", rota[i]);
    }
    putchar('\n');
}

static int demonstrar_hamiltoniano(void)
{
    enum { N = 4 };
    int adj[N * N] = {0};
    int pesos[N * N], rota[N + 1];
    adicionar_aresta(N, adj, 1, 0);
    adicionar_aresta(N, adj, 0, 2);
    adicionar_aresta(N, adj, 2, 3);

    puts("\nGrafo em cadeia:");
    if (encontrar_caminho_hamiltoniano_bt(N, adj, rota) != 1) {
        return 0;
    }
    imprimir_rota("Caminho hamiltoniano: ", N, rota);
    if (encontrar_ciclo_hamiltoniano_bt(N, adj, rota) != 0 ||
        grafo_para_tsp(N, adj, pesos) != 1) {
        return 0;
    }
    long long custo = tsp_forca_bruta(N, pesos, 0, rota);
    if (custo <= N) {
        return 0;
    }
    printf("Sem ciclo hamiltoniano. Custo otimo da reducao para TSP: %lld.\n", custo);

    adicionar_aresta(N, adj, 3, 1);
    puts("\nCom a aresta entre 3 e 1:");
    if (encontrar_ciclo_hamiltoniano_bt(N, adj, rota) != 1) {
        return 0;
    }
    imprimir_rota("Ciclo hamiltoniano: ", N + 1, rota);
    if (grafo_para_tsp(N, adj, pesos) != 1 ||
        tsp_forca_bruta(N, pesos, 0, rota) != N) {
        return 0;
    }
    printf("Custo otimo da reducao para TSP: %d.\n", N);
    return 1;
}

static uint32_t sortear(uint32_t *estado)
{
    *estado ^= *estado << 13;
    *estado ^= *estado >> 17;
    *estado ^= *estado << 5;
    return *estado;
}

enum { MAX_N = 12 };

static void gerar_grafo(int n, int *adj)
{
    uint32_t estado = 0x9e3779b9u + (uint32_t)n;
    int ordem[MAX_N];
    for (int u = 0; u < n; ++u) {
        ordem[u] = u;
        for (int v = u + 1; v < n; ++v) {
            if (sortear(&estado) % 100u < 25u) {
                adicionar_aresta(n, adj, u, v);
            }
        }
    }
    for (int i = n - 1; i > 0; --i) {
        int j = (int)(sortear(&estado) % (uint32_t)(i + 1));
        int temp = ordem[i];
        ordem[i] = ordem[j];
        ordem[j] = temp;
    }
    /* Garante um ciclo hamiltoniano e custo otimo n apos a reducao. */
    for (int i = 0; i < n; ++i) {
        adicionar_aresta(n, adj, ordem[i], ordem[(i + 1) % n]);
    }
}

typedef long long (*AlgoritmoTsp)(int, const int *, int, int *);

static long long medir(AlgoritmoTsp algoritmo, int n, const int *pesos,
                       double *tempo_us)
{
    int rota[MAX_N + 1];
    clock_t inicio = clock();
    if (inicio == (clock_t)-1) {
        return -1;
    }
    unsigned long repeticoes = 0;
    int lote = 1;
    double segundos;
    long long custo = -1;
    do {
        for (int i = 0; i < lote; ++i) {
            custo = algoritmo(n, pesos, 0, rota);
            if (custo < 0) {
                return -1;
            }
        }
        repeticoes += (unsigned long)lote;
        clock_t fim = clock();
        if (fim == (clock_t)-1 || fim < inicio) {
            return -1;
        }
        segundos = ((double)fim - (double)inicio) / CLOCKS_PER_SEC;
        /* Comeca com uma execucao para o caso fatorial e aumenta o lote
           para que as heuristicas gastem menos tempo consultando o relogio. */
        if (lote < 256) {
            lote *= 2;
        }
    } while (segundos < 0.05);
    *tempo_us = 1000000.0 * segundos / repeticoes;
    return custo;
}

static int executar_experimento(void)
{
    const int tamanhos[] = {5, 8, 10, MAX_N};
    puts("\nTSP com pesos 1 e 2. Os metodos usam o mesmo grafo em cada n.");
    puts("Tempos medios por execucao em microssegundos (us), medidos com clock().");
    puts(" n | Forca bruta (custo / us) | Vizinho proximo (custo / us) | Insercao (custo / us)");
    for (size_t i = 0; i < sizeof(tamanhos) / sizeof(tamanhos[0]); ++i) {
        int n = tamanhos[i];
        int adj[MAX_N * MAX_N] = {0}, pesos[MAX_N * MAX_N];
        gerar_grafo(n, adj);
        if (grafo_para_tsp(n, adj, pesos) != 1) {
            return 0;
        }

        double tempo_exato, tempo_vizinho, tempo_insercao;
        long long exato = medir(tsp_forca_bruta, n, pesos, &tempo_exato);
        long long vizinho = medir(tsp_vizinho_mais_proximo, n, pesos, &tempo_vizinho);
        long long insercao = medir(tsp_insercao, n, pesos, &tempo_insercao);
        if (exato != n || vizinho < exato || insercao < exato) {
            return 0;
        }
        printf("%2d | %5lld / %15.3f | %5lld / %18.3f | %5lld / %11.3f\n",
               n, exato, tempo_exato, vizinho, tempo_vizinho, insercao, tempo_insercao);
    }
    return 1;
}

int main(void)
{
    puts("Pratica 10 - Caminho Hamiltoniano e TSP");
    if (!demonstrar_hamiltoniano() || !executar_experimento()) {
        fputs("Erro ao verificar a demonstracao ou medir o experimento.\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
