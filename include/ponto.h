#ifndef PONTO_H
#define PONTO_H

typedef struct
{
    char *idend;
    long id_logrado;
    char *sigla_tipo;
    char *nome_logra;
    int numero_imo;
    char *nome_bairr;
    char *nome_regio;
    int cep;
    double x;
    double y;
} addr_t, *ptr_addr_t;

typedef struct
{
    double x, y;
} Ponto;

// Função para criar um ponto
Ponto criaPonto(double x, double y);

// Função para calcular a distância entre dois pontos
double distancia(const addr_t *p, const Ponto *b);

#endif
