#include "../include/heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cria um objeto de distancia ponto
DistanciaPonto criaDistanciaPonto(double d, addr_t *p)
{
    DistanciaPonto dp;
    dp.distancia = d;
    dp.ponto = p;
    return dp;
}

// Função que cria o maxHeap
MaxHeap *criaMaxHeap(int cap)
{
    MaxHeap *h = (MaxHeap *)malloc(sizeof(MaxHeap));
    h->capacidade = cap;
    h->tamanho = 0;
    h->heap = (DistanciaPonto *)malloc(cap * sizeof(DistanciaPonto));
    return h;
}

// Função que destrio o maxHeap
void destroiMaxHeap(MaxHeap *h)
{
    free(h->heap);
    free(h);
}

// Função que restaura a propriedade da MaxHeap movendo um elemento para cima após a inserção
void heapifyUp(MaxHeap *h, int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;
        if (h->heap[parent].distancia >= h->heap[index].distancia)
            break;
        DistanciaPonto temp = h->heap[parent];
        h->heap[parent] = h->heap[index];
        h->heap[index] = temp;
        index = parent;
    }
}

// Função que restaura a propriedade da MaxHeap movendo um elemento para baixo após a remoção
void heapifyDown(MaxHeap *h, int index)
{
    while (2 * index + 1 < h->tamanho)
    {
        int esq = 2 * index + 1;
        int dir = 2 * index + 2;
        int maior = esq;

        if (dir < h->tamanho && h->heap[dir].distancia > h->heap[esq].distancia)
        {
            maior = dir;
        }

        if (h->heap[index].distancia >= h->heap[maior].distancia)
            break;

        DistanciaPonto temp = h->heap[index];
        h->heap[index] = h->heap[maior];
        h->heap[maior] = temp;
        index = maior;
    }
}

// Função para inserir um elemento no heap
void push(MaxHeap *h, double distancia, addr_t *ponto)
{
    if (h->tamanho < h->capacidade)
    {
        h->heap[h->tamanho++] = criaDistanciaPonto(distancia, ponto);
        heapifyUp(h, h->tamanho - 1);
    }
    else if (distancia < h->heap[0].distancia)
    {
        h->heap[0] = criaDistanciaPonto(distancia, ponto);
        heapifyDown(h, 0);
    }
}

// Retorna o tamanho do heap
int size(MaxHeap *h)
{
    return h->tamanho;
}

// Retorna o elemento do topo, de maior distancia do heap
DistanciaPonto top(MaxHeap *h)
{
    return h->heap[0];
}

// Exclui o elemento do topo do heap
void pop(MaxHeap *h)
{
    if (h->tamanho > 0)
    {
        h->heap[0] = h->heap[--h->tamanho];
        heapifyDown(h, 0);
    }
}

// Retorna o elemento de uma posição do heap
DistanciaPonto getItem(MaxHeap *h, int index)
{
    return h->heap[index];
}

// Função para imprimir em ordem crescente o heap
void imprimeHeapEmOrdemCrescente(MaxHeap *h)
{
    if (h == NULL || h->tamanho == 0)
    {
        return;
    }

    // Cria uma cópia do heap para não modificar o heap original
    MaxHeap *copiaHeap = criaMaxHeap(h->capacidade);
    memcpy(copiaHeap->heap, h->heap, h->tamanho * sizeof(DistanciaPonto));
    copiaHeap->tamanho = h->tamanho;

    // Cria uma lista para armazenar os elementos em ordem crescente
    DistanciaPonto *ordenados = (DistanciaPonto *)malloc(h->tamanho * sizeof(DistanciaPonto));
    int n = 0;

    // Remove os elementos da heap (maiores primeiro) e armazena na lista
    while (copiaHeap->tamanho > 0)
    {
        ordenados[n++] = top(copiaHeap);
        pop(copiaHeap);
    }

    // Imprime os elementos na ordem crescente (que agora está no vetor "ordenados")
    for (int i = n - 1; i >= 0; --i)
    {
        DistanciaPonto dp = ordenados[i];
        printf("%s %s, %d, %s, %s, %d", dp.ponto->sigla_tipo,
               dp.ponto->nome_logra, dp.ponto->numero_imo,
               dp.ponto->nome_bairr, dp.ponto->nome_regio,
               dp.ponto->cep);
        printf(" (%.3f)\n", dp.distancia);
    }

    // Libera a memória utilizada
    free(ordenados);
    destroiMaxHeap(copiaHeap);
}