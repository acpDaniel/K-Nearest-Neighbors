#ifndef QUADTREE_H
#define QUADTREE_H

#include "heap.h"
#include "ponto.h"
#include "smv.h"

typedef struct
{
    addr_t point; // Informações ponto de recarga
    int hasPoint; // Indicador de presença de estação
    int active;   // Indicador de ativação

    // variáveis para armazenar o índice dos seguintes elementos
    int parent;
    int nw;
    int ne;
    int sw;
    int se;
} Node;

typedef struct
{
    Node *nodes;    // Array dinâmico de nós
    int capacidade; // Capacidade do array
    int tamanho;    // Quantidade atual de nós
} QuadTree;

// Funções associadas à QuadTree
QuadTree *criaQuadTree(int initialCapacity);
void destroiQuadTree(QuadTree *qt);
int insert(QuadTree *qt, addr_t point);
int insertIndex(QuadTree *qt, int nodeIndex, int parentIndex, int directionId, addr_t point);
void resize(QuadTree *qt);
int activate(QuadTree *qt, char *idend, double x, double y);
int deactivate(QuadTree *qt, char *idend, double x, double y);
int activateDeactivate(QuadTree *qt, int nodeIndex, double x, double y, int activate);
void buscaKNN(QuadTree *qt, const Ponto *p, int K, MaxHeap *fp);
void buscaKNNRecursivo(QuadTree *qt, int nodeIndex, const Ponto *p, int K, MaxHeap *fp);
void buscaKNNRecursivoOtimizado(QuadTree *qt, int nodeIndex, const Ponto *p, int K, MaxHeap *fp);
double distanciaMinimaRegiao(Node *node, const Ponto *p);

#endif // QUADTREE_H
