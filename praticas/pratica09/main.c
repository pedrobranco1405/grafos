#include "cobertura.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef int (*AlgoritmoCobertura)(int, int, const Aresta *, int *);

static void imprimir_conjunto(const char *nome, int n, int quantidade,
                              const int *selecionados)
{
    printf("%s (%d vertices): {", nome, quantidade);
    const char *separador = "";
    for (int v = 0; v < n; ++v) {
        if (selecionados[v]) {
            printf("%s%d", separador, v);
            separador = ", ";
        }
    }
    puts("}");
}

static int demonstrar_coberturas(void)
{
    /* O vertice 0 empata em grau com 1, 2 e 3, e e escolhido primeiro pela
     * heuristica. A cobertura otima tem 3 vertices, enquanto a gulosa tem 4.
     */
    const Aresta arestas[] = {
        {0, 1}, {0, 2}, {0, 3}, {1, 4}, {1, 5},
        {2, 6}, {2, 7}, {3, 8}, {3, 9}
    };
    int otima[10], gulosa[10];
    int tamanho_otimo = cobertura_minima_bt(10, 9, arestas, otima);
    int tamanho_guloso = cobertura_gulosa(10, 9, arestas, gulosa);
    if (tamanho_otimo < 0 || tamanho_guloso < 0 ||
        eh_cobertura(10, 9, arestas, otima) != 1 ||
        eh_cobertura(10, 9, arestas, gulosa) != 1) {
        return 0;
    }
    puts("\nExemplo: a heuristica pode selecionar mais vertices que o otimo.");
    imprimir_conjunto("Backtracking", 10, tamanho_otimo, otima);
    imprimir_conjunto("Maior grau restante", 10, tamanho_guloso, gulosa);
    return 1;
}

static uint32_t proximo_aleatorio(uint32_t *estado)
{
    *estado ^= *estado << 13;
    *estado ^= *estado >> 17;
    *estado ^= *estado << 5;
    return *estado;
}

static int gerar_grafo(int n, Aresta *arestas)
{
    /* Semente fixa por n: caminho 0..n-1 mais arestas extras com chance 35%.
     * Os dois algoritmos recebem exatamente o mesmo grafo em cada tamanho.
     */
    uint32_t estado = 0x9e3779b9u + (uint32_t)n;
    int m = 0;
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            uint32_t sorteio = proximo_aleatorio(&estado) % 100u;
            if (v == u + 1 || sorteio < 35u) {
                arestas[m++] = (Aresta){u, v};
            }
        }
    }
    return m;
}

static int medir_tempo(AlgoritmoCobertura algoritmo, int n, int m,
                       const Aresta *arestas, int *selecionados,
                       double *tempo_us, unsigned long *repeticoes)
{
    clock_t inicio = clock();
    if (inicio == (clock_t)-1) {
        return -1;
    }
    *repeticoes = 0;
    int tamanho = -1;
    double segundos;
    do {
        /* Lotes amortizam a resolucao do relogio e o custo de consultar clock. */
        for (int i = 0; i < 100; ++i) {
            tamanho = algoritmo(n, m, arestas, selecionados);
            if (tamanho < 0) {
                return -1;
            }
            ++*repeticoes;
        }
        clock_t fim = clock();
        if (fim == (clock_t)-1 || fim < inicio) {
            return -1;
        }
        segundos = ((double)fim - (double)inicio) / CLOCKS_PER_SEC;
    } while (segundos < 0.05);
    *tempo_us = segundos * 1000000.0 / *repeticoes;
    return tamanho;
}

static int executar_experimento(void)
{
    const int tamanhos[] = {5, 8, 10, 12, 15};
    puts("\nExperimento: medias por execucao, medidas com clock().");
    puts("Lotes de 100 execucoes; minimo de 50 ms por algoritmo e tamanho.");
    puts(" n |   m | Otima | Gulosa |     BT (us) | Gulosa (us) | Repeticoes BT/Gulosa");
    for (size_t i = 0; i < sizeof(tamanhos) / sizeof(tamanhos[0]); ++i) {
        int n = tamanhos[i];
        Aresta arestas[105]; /* 15 * 14 / 2: maximo de arestas do experimento. */
        int otima[15], gulosa[15];
        int m = gerar_grafo(n, arestas);
        double tempo_bt, tempo_guloso;
        unsigned long repeticoes_bt, repeticoes_gulosas;
        int tamanho_otimo = medir_tempo(cobertura_minima_bt, n, m, arestas, otima,
                                        &tempo_bt, &repeticoes_bt);
        int tamanho_guloso = medir_tempo(cobertura_gulosa, n, m, arestas, gulosa,
                                         &tempo_guloso, &repeticoes_gulosas);
        if (tamanho_otimo < 0 || tamanho_guloso < tamanho_otimo ||
            eh_cobertura(n, m, arestas, otima) != 1 ||
            eh_cobertura(n, m, arestas, gulosa) != 1) {
            return 0;
        }
        printf("%2d | %3d | %5d | %6d | %11.3f | %11.3f | %lu/%lu\n",
               n, m, tamanho_otimo, tamanho_guloso, tempo_bt, tempo_guloso,
               repeticoes_bt, repeticoes_gulosas);
    }
    puts("BT encontra o minimo; a heuristica e rapida, mas pode usar mais vertices.");
    puts("Tempos variam com a maquina e o grafo; as medias nao sao garantias gerais.");
    return 1;
}

int main(void)
{
    puts("Pratica 09 - Cobertura de Vertices");
    if (!demonstrar_coberturas() || !executar_experimento()) {
        fputs("Erro: dados, memoria, relogio ou cobertura invalida.\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
