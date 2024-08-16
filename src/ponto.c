#include "../include/ponto.h"
#include <math.h>

// Retorna um pont de coordenadas
Ponto criaPonto(double x, double y)
{
    Ponto p;
    p.x = x;
    p.y = y;
    return p;
}

// Retorna a distancia eucidiana entre uma estação e um ponto de coordenadas
double distancia(addr_t *p, Ponto *b)
{
    return sqrt((p->x - b->x) * (p->x - b->x) + (p->y - b->y) * (p->y - b->y));
}
