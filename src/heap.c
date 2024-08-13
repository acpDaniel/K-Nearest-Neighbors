#include "../include/heap.h"
#include <stdlib.h>

DistanciaPonto criaDistanciaPonto(double d, addr_t *p)
{
    DistanciaPonto dp;
    dp.distancia = d;
    dp.ponto = p;
    return dp;
}

MaxHeap *criaMaxHeap(int cap)
{
    MaxHeap *h = (MaxHeap *)malloc(sizeof(MaxHeap));
    h->capacidade = cap;
    h->tamanho = 0;
    h->heap = (DistanciaPonto *)malloc(cap * sizeof(DistanciaPonto));
    return h;
}

void destroiMaxHeap(MaxHeap *h)
{
    free(h->heap);
    free(h);
}

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

void heapifyDown(MaxHeap *h, int index)
{
    while (2 * index + 1 < h->tamanho)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int maior = left;

        if (right < h->tamanho && h->heap[right].distancia > h->heap[left].distancia)
        {
            maior = right;
        }

        if (h->heap[index].distancia >= h->heap[maior].distancia)
            break;

        DistanciaPonto temp = h->heap[index];
        h->heap[index] = h->heap[maior];
        h->heap[maior] = temp;
        index = maior;
    }
}

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

int size(MaxHeap *h)
{
    return h->tamanho;
}

DistanciaPonto top(MaxHeap *h)
{
    return h->heap[0];
}

void pop(MaxHeap *h)
{
    if (h->tamanho > 0)
    {
        h->heap[0] = h->heap[--h->tamanho];
        heapifyDown(h, 0);
    }
}

DistanciaPonto getItem(MaxHeap *h, int index)
{
    return h->heap[index];
}
