#include "../include/quadTree.h"
#include <stdlib.h>
#include <stdio.h>

QuadTree *criaQuadTree(int initialCapacity)
{
    QuadTree *qt = (QuadTree *)malloc(1 * sizeof(QuadTree));
    qt->capacidade = initialCapacity;
    qt->tamanho = 0;
    qt->nodes = (Node *)calloc(initialCapacity, sizeof(Node));

    // Inicializa cada nó
    // for (int i = 0; i < initialCapacity; ++i)
    // {
    //     qt->nodes[i].hasPoint = 0;
    //     qt->nodes[i].active = 0;
    //     qt->nodes[i].parent = -1;
    //     qt->nodes[i].nw = -1;
    //     qt->nodes[i].ne = -1;
    //     qt->nodes[i].sw = -1;
    //     qt->nodes[i].se = -1;
    // }
    return qt;
}

void destroiQuadTree(QuadTree *qt)
{
    // end_page();
}

int insert(QuadTree *qt, addr_t point)
{
    return insertIndex(qt, 0, -1, 0, point);
}

int insertIndex(QuadTree *qt, int nodeIndex, int parentIndex, int directionId, addr_t point)
{
    if (nodeIndex < 0 || nodeIndex >= qt->capacidade)
    {
        printf("Indice invalido %.2i, capacidade: %.2i", nodeIndex, qt->capacidade);
        return 0; // Índice inválido
    }

    // printf("Testando ponto no indice %.2i \n", nodeIndex);
    Node *node = &(qt->nodes[nodeIndex]);

    if (!node->hasPoint)
    {
        qt->tamanho++;
        node->point = point;
        node->hasPoint = 1;
        node->active = 1;
        node->parent = parentIndex;

        if (directionId != 0)
        {
            Node *parentNode = &(qt->nodes[parentIndex]);
            // NW = 1 , NE = 2 , SW = 3 , SE = 4
            switch (directionId)
            {
            case 1:
                parentNode->nw = nodeIndex;
                break;
            case 2:
                parentNode->ne = nodeIndex;
                break;
            case 3:
                parentNode->sw = nodeIndex;
                break;
            case 4:
                parentNode->se = nodeIndex;
                break;
            }
        }

        return 1;
    }

    int isWest = point.x < node->point.x;
    int isNorth = point.y > node->point.y;

    // NW = 1 , NE = 2 , SW = 3 , SE = 4
    int childDirection = isNorth ? (isWest ? 1 : 2) : (isWest ? 3 : 4);
    int childIndex = 0;

    switch (childDirection)
    {
    case 1:
        childIndex = node->nw;
        break;
    case 2:
        childIndex = node->ne;
        break;
    case 3:
        childIndex = node->sw;
        break;
    case 4:
        childIndex = node->se;
        break;
    }

    // se não tem filho, iremos inserir na proxima posicao vazia do vetor de nodes
    if (childIndex == 0)
    {
        childIndex = qt->tamanho;
    }

    if (qt->tamanho == qt->capacidade)
    {
        // printf("Entrou no resize \n");
        resize(qt);
        // printf("capacidade apos resize: %.2i \n", qt->capacidade);
    }

    return insertIndex(qt, childIndex, nodeIndex, childDirection, point);
}

void resize(QuadTree *qt)
{
    int newCapacity = qt->capacidade * 2;
    Node *newNodes = (Node *)calloc(newCapacity, sizeof(Node));

    if (!newNodes)
    {
        // fprintf(stderr, "Erro ao alocar memória para redimensionar a QuadTree\n");
        exit(EXIT_FAILURE);
    }

    // Copia os nós existentes para o novo array
    for (int i = 0; i < qt->capacidade; i++)
    {
        newNodes[i] = qt->nodes[i];
    }

    // Libere a memória antiga e atualize o ponteiro
    free(qt->nodes);
    qt->nodes = newNodes;
    qt->capacidade = newCapacity;
}

int activate(QuadTree *qt, char *idend, double x, double y)
{
    int result = activateDeactivate(qt, 0, x, y, 1); // Alterado para 1 (ativar)

    if (result == 0)
    {
        printf("Ponto de recarga %s já estava ativo.\n", idend);
    }
    else
    {
        printf("Ponto de recarga %s ativado.\n", idend);
    }

    return result;
}

int deactivate(QuadTree *qt, char *idend, double x, double y)
{
    int result = activateDeactivate(qt, 0, x, y, 0); // Alterado para 0 (desativar)

    if (result == 0)
    {
        printf("Ponto de recarga %s já estava desativado.\n", idend);
    }
    else
    {
        printf("Ponto de recarga %s desativado.\n", idend);
    }

    return result;
}

int activateDeactivate(QuadTree *qt, int nodeIndex, double x, double y, int activate)
{
    Node *node = &(qt->nodes[nodeIndex]);

    if ((nodeIndex < 0 || nodeIndex >= qt->capacidade) || (!node->hasPoint))
    {
        return 0;
    }

    if (node->hasPoint && node->point.x == x && node->point.y == y)
    {
        if (node->active == activate)
        {
            return 0;
        }
        node->active = activate;
        return 1;
    }

    int isWest = x < node->point.x;
    int isNorth = y > node->point.y;

    // NW = 1 , NE = 2 , SW = 3 , SE = 4
    int childDirection = isNorth ? (isWest ? 1 : 2) : (isWest ? 3 : 4);
    int childIndex = 0;

    switch (childDirection)
    {
    case 1:
        childIndex = node->nw;
        break;
    case 2:
        childIndex = node->ne;
        break;
    case 3:
        childIndex = node->sw;
        break;
    case 4:
        childIndex = node->se;
        break;
    }

    if (childIndex == 0)
    {
        childIndex = qt->tamanho;
    }

    return activateDeactivate(qt, childIndex, x, y, activate);
}

void buscaKNN(QuadTree *qt, const Ponto *p, int K, MaxHeap *fp)
{
    buscaKNNRecursivo(qt, 0, p, K, fp);
}

void buscaKNNRecursivo(QuadTree *qt, int nodeIndex, const Ponto *p, int K, MaxHeap *fp)
{
    if (nodeIndex < 0 || nodeIndex >= qt->capacidade)
    {
        return;
    }

    Node *node = &(qt->nodes[nodeIndex]);

    if (node->hasPoint && node->active)
    {
        double dist = distancia(&(node->point), p);

        if (size(fp) < K)
        {
            push(fp, dist, &(node->point));
        }
        else if (dist < top(fp).distancia)
        {
            pop(fp);
            push(fp, dist, &(node->point));
        }
    }

    buscaKNNRecursivo(qt, nodeIndex + 1, p, K, fp);
}

void buscaKNNRecursivoOtimizado(QuadTree *qt, int nodeIndex, const Ponto *p, int K, MaxHeap *fp)
{
    if (nodeIndex < 0 || nodeIndex >= qt->capacidade)
    {
        return;
    }

    Node *node = &(qt->nodes[nodeIndex]);

    // Se o nó não tiver ponto ou não estiver ativo, ignorar
    if (node->hasPoint && node->active)
    {
        // Calcular a distância do ponto atual ao ponto de referência
        double dist = distancia(&(node->point), p);

        // Se há espaço no heap ou o ponto atual está mais próximo do que o mais distante no heap
        if (size(fp) < K)
        {
            push(fp, dist, &(node->point));
        }
        else if (dist < top(fp).distancia)
        {
            pop(fp);
            push(fp, dist, &(node->point));
        }
    }

    // Agora, verificamos cada sub-região recursivamente, mas com uma lógica que tenta otimizar a busca
    // Sub-regiões são verificadas na ordem da proximidade ao ponto de interesse

    int childIndices[4] = {node->nw, node->ne, node->sw, node->se};

    for (int i = 0; i < 4; ++i)
    {
        int childIndex = childIndices[i];
        if (childIndex > 0 && childIndex < qt->capacidade)
        {
            // Verificamos a distância mínima entre o ponto e a sub-região
            double minDist = distanciaMinimaRegiao(&(qt->nodes[childIndex]), p);

            // Se a menor distância na sub-região é menor do que a maior distância no heap, explore essa sub-região
            if (size(fp) < K || minDist < top(fp).distancia)
            {
                buscaKNNRecursivoOtimizado(qt, childIndex, p, K, fp);
            }
        }
    }
}

double distanciaMinimaRegiao(Node *node, const Ponto *p)
{
    double dx = fmax(0, fmax(node->point.x - p->x, p->x - node->point.x));
    double dy = fmax(0, fmax(node->point.y - p->y, p->y - node->point.y));
    return sqrt(dx * dx + dy * dy);
}
