#include "hamiltoniano.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    int n;
    const int *adj;
    int *ordem;
    unsigned char *visitado;
    int fechar_ciclo;
} Busca;

static int buscar(Busca *busca, int tamanho)
{
    int ultimo = busca->ordem[tamanho - 1];
    const int *vizinhos = busca->adj + (size_t)ultimo * busca->n;
    if (tamanho == busca->n) {
        return !busca->fechar_ciclo || vizinhos[busca->ordem[0]] != 0;
    }

    for (int v = 0; v < busca->n; ++v) {
        if (!busca->visitado[v] && vizinhos[v] != 0) {
            busca->ordem[tamanho] = v;
            busca->visitado[v] = 1;
            if (buscar(busca, tamanho + 1)) {
                return 1;
            }
            busca->visitado[v] = 0;
        }
    }
    return 0;
}

static int encontrar(int n, const int *adj, int *ordem, int fechar_ciclo)
{
    if (n <= 0 || adj == NULL || ordem == NULL ||
        (size_t)n > SIZE_MAX / sizeof(*adj) / (size_t)n) {
        return -1;
    }
    if (fechar_ciclo && n < 3) {
        return 0;
    }
    unsigned char *visitado = calloc((size_t)n, sizeof(*visitado));
    if (visitado == NULL) {
        return -1;
    }

    Busca busca = {n, adj, ordem, visitado, fechar_ciclo};
    int encontrou = 0;
    /* Um ciclo pode ser rotacionado para comecar em 0. Um caminho pode
       precisar de outra origem, entao testamos todos os vertices iniciais. */
    int origens = fechar_ciclo ? 1 : n;
    for (int v = 0; v < origens && !encontrou; ++v) {
        ordem[0] = v;
        visitado[v] = 1;
        encontrou = buscar(&busca, 1);
        visitado[v] = 0;
    }
    if (encontrou && fechar_ciclo) {
        ordem[n] = ordem[0];
    }
    free(visitado);
    return encontrou;
}

int encontrar_caminho_hamiltoniano_bt(int n, const int *adj, int *caminho)
{
    return encontrar(n, adj, caminho, 0);
}

int encontrar_ciclo_hamiltoniano_bt(int n, const int *adj, int *ciclo)
{
    return encontrar(n, adj, ciclo, 1);
}
