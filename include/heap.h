#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>
#include <math.h>
#include "ponto.h"

// Estrutura de par de ponto e distância
typedef struct
{
    double distancia;
    addr_t *ponto;
} DistanciaPonto;

// Função para criar um DistanciaPonto
DistanciaPonto criaDistanciaPonto(double d, addr_t *p);

// Estrutura MaxHeap
typedef struct
{
    DistanciaPonto *heap;
    int capacidade;
    int tamanho;
} MaxHeap;

// Funções associadas à MaxHeap
MaxHeap *criaMaxHeap(int cap);
void destroiMaxHeap(MaxHeap *h);
void heapifyUp(MaxHeap *h, int index);
void heapifyDown(MaxHeap *h, int index);
void push(MaxHeap *h, double distancia, addr_t *ponto);
int size(MaxHeap *h);
DistanciaPonto top(MaxHeap *h);
void pop(MaxHeap *h);
DistanciaPonto getItem(MaxHeap *h, int index);
int comparaDistancia(const void *a, const void *b);
void imprimeHeapEmOrdemCrescente(MaxHeap *h);

#endif
