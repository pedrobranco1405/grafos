# Pratica 06 - Menor caminho com Dijkstra

Implementacao em C11 de um grafo ponderado por listas de adjacencia,
Min-Heap e reconstrucao de caminhos. Os vertices sao numerados a partir de 0.

## Compilar, executar e testar

No PowerShell, na pasta desta pratica:

```powershell
Set-Location 'C:\Users\User\Desktop\Git IESB\Grafos\grafos\praticas\pratica06'
mingw32-make
.\main.exe
mingw32-make test
```

No Linux/macOS, use `make`, `./main` e `make test`. `make run` executa o
exemplo e `make clean` remove os executaveis. Tambem e possivel compilar
diretamente com `gcc -Wall -Wextra -Werror -std=c11 main.c dijkstra.c -o main`.

## Funcionamento

- `inserir_min_heap`: insere um vertice e sobe enquanto sua prioridade for
  menor que a do pai.
- `extrair_minimo`: remove a raiz, move o ultimo elemento para ela e desce
  ate restaurar a propriedade do heap.
- `diminuir_chave`: localiza o vertice por `posicao[vertice]` em O(1), altera
  sua prioridade e sobe em O(log V). Cada troca atualiza o mapeamento.
- `dijkstra`: inicializa as distancias com infinito, retira o vertice de menor
  distancia e relaxa suas arestas. Cada melhoria registra o predecessor e
  diminui a chave no heap. Vertices extraidos nao sao reabertos.
- `reconstruir_caminho`: percorre os predecessores do destino ate a origem,
  armazena o percurso e o imprime na ordem correta.

O heap armazena no maximo V vertices. Dijkstra custa O((V + E) log V),
com memoria auxiliar O(V); o grafo ocupa O(V + E).

Pesos devem ser nao negativos; peso zero e permitido. `INFINITO` (`INT_MAX`)
fica reservado para ausencia de custo representavel. A soma e verificada
antes da adicao para evitar overflow: custos >= `INT_MAX` permanecem infinitos.
`pred` deve ser o vetor produzido para a mesma origem ao reconstruir caminhos.
O grafo pode ser orientado (`criar_grafo_ponderado(n, 1)`) ou nao orientado
(`criar_grafo_ponderado(n, 0)`).

## Resultado esperado do exemplo

Origem 0; distancias para os vertices 0 a 6: `0, 7, 9, 20, 20, 11, INF`.
O caminho para 4 e `0 -> 2 -> 5 -> 4`, com custo 20. O caminho de 0 para
ele mesmo e `0`; o vertice 6 esta isolado e exibe uma mensagem de ausencia
de caminho.

Os testes verificam operacoes e mapeamento do heap, pesos zero, ciclos,
arestas paralelas, vertices isolados, origem igual ao destino, argumentos
invalidos, rejeicao de pesos negativos e protecao contra overflow. Tambem
comparam 250 grafos orientados e nao orientados com uma referencia independente
de Bellman-Ford, usando todas as origens e conferindo os predecessores.

Commit e push ficam a cargo do aluno, na branch `pratica-06`. Depois do envio,
o fluxo do enunciado exige PR para `main`, Issue vinculada e check do Actions.
