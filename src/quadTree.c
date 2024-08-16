#include "../include/quadTree.h"
#include <stdlib.h>
#include <stdio.h>

// Função para criar uma quadTree
QuadTree *criaQuadTree(int initialCapacity, double minX, double minY, double maxX, double maxY)
{
    QuadTree *qt = (QuadTree *)malloc(1 * sizeof(QuadTree));
    qt->capacidade = initialCapacity;
    qt->tamanho = 0;
    qt->nodes = (Node *)calloc(initialCapacity, sizeof(Node));

    // Inicializa a raiz da QuadTree com os limites dados
    qt->nodes[0].minX = minX;
    qt->nodes[0].minY = minY;
    qt->nodes[0].maxX = maxX;
    qt->nodes[0].maxY = maxY;
    return qt;
}

// Função para desalocar a quadTree
void destroiQuadTree(QuadTree *qt)
{
    if (qt == NULL)
        return;

    free(qt->nodes);
    free(qt);
}

// Inserir uma nova estação de carga na quadTree
int insert(QuadTree *qt, addr_t point)
{
    return insertIndex(qt, 0, -1, 0, point);
}

// Função recursiva para inserir a estação com os indices corretos
int insertIndex(QuadTree *qt, int nodeIndex, int parentIndex, int directionId, addr_t point)
{
    if (nodeIndex < 0 || nodeIndex >= qt->capacidade)
    {
        return 0; // Índice inválido
    }

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
                node->maxX = parentNode->point.x;
                node->minY = parentNode->point.y;
                node->minX = parentNode->minX;
                node->maxY = parentNode->maxY;
                break;
            case 2:
                parentNode->ne = nodeIndex;
                node->minX = parentNode->point.x;
                node->minY = parentNode->point.y;
                node->maxX = parentNode->maxX;
                node->maxY = parentNode->maxY;
                break;
            case 3:
                parentNode->sw = nodeIndex;
                node->maxX = parentNode->point.x;
                node->maxY = parentNode->point.y;
                node->minX = parentNode->minX;
                node->minY = parentNode->minY;
                break;
            case 4:
                parentNode->se = nodeIndex;
                node->minX = parentNode->point.x;
                node->maxY = parentNode->point.y;
                node->maxX = parentNode->maxX;
                node->minY = parentNode->minY;
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
        resize(qt);
    }

    return insertIndex(qt, childIndex, nodeIndex, childDirection, point);
}

// Função para dar resize no vetor de nós da quadTree
void resize(QuadTree *qt)
{
    int newCapacity = qt->capacidade * 2;
    Node *newNodes = (Node *)calloc(newCapacity, sizeof(Node));

    if (!newNodes)
    {
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

// Função para ativar uma estação de recarga
int activate(QuadTree *qt, char *idend, double x, double y)
{
    // Alterado para 1 (ativar)
    int result = activateDeactivate(qt, 0, x, y, 1);

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

// Função para desativar uma estação de recarga
int deactivate(QuadTree *qt, char *idend, double x, double y)
{
    // Alterado para 0 (desativar)
    int result = activateDeactivate(qt, 0, x, y, 0);

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

// Função que é utilziada tanto para ativar quanto desativar uma estação
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

// Função linear para busca do pontos mais proximos
void buscaKNN(QuadTree *qt, Ponto *p, int K, MaxHeap *fp)
{
    buscaKNNRecursivo(qt, 0, p, K, fp);
}

// Função linear e recursiva para busca do pontos mais proximos, apenas para comparação de resultados
void buscaKNNRecursivo(QuadTree *qt, int nodeIndex, Ponto *p, int K, MaxHeap *fp)
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

    int childIndices[4] = {node->nw, node->ne, node->sw, node->se};

    for (int i = 0; i < 4; ++i)
    {
        int childIndex = childIndices[i];
        if (childIndex > 0 && childIndex < qt->capacidade)
        {
            buscaKNNRecursivo(qt, childIndex, p, K, fp);
        }
    }
}

// Função recusriva e com uma otimização para retornar os pontos mais próximos mais rapidamente
void buscaKNNRecursivoOtimizado(QuadTree *qt, int nodeIndex, Ponto *p, int K, MaxHeap *fp)
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

// Calcula a distância mínima entre um ponto e a região representada por um nó da QuadTree
double distanciaMinimaRegiao(Node *node, Ponto *p)
{
    double dx = fmax(0, fmax(node->minX - p->x, p->x - node->maxX));
    double dy = fmax(0, fmax(node->minY - p->y, p->y - node->maxY));
    return sqrt(dx * dx + dy * dy);
}
