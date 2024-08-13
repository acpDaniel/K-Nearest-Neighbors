#include "../include/ponto.h"
#include <math.h>

Ponto criaPonto(double x, double y)
{
    Ponto p;
    p.x = x;
    p.y = y;
    return p;
}

double distancia(const addr_t *p, const Ponto *b)
{
    return sqrt((p->x - b->x) * (p->x - b->x) + (p->y - b->y) * (p->y - b->y));
}
