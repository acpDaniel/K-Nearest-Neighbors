#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura de ElementoTabela
typedef struct
{
    char *id;      // ID como string
    double x, y;   // Coordenadas associadas
    bool vazio;    // Indica se a posição está vazia
    bool retirada; // Indica se o elemento foi retirado
} ElementoTabela;

// Estrutura de HashTable
typedef struct
{
    ElementoTabela *tabela; // Ponteiro para os elementos da tabela
    int tamanhoConjunto;    // Número de elementos na tabela
    int tamanhoTabela;      // Tamanho atual da tabela
    int tamanhoOriginal;    // Tamanho original da tabela
} HashTable;

// Declaração das funções
HashTable criaHashTable(int tamanhoOriginal);
void destroiHashTable(HashTable *ht);
int Hash(char *s, int tamanho);
void Resize(HashTable *ht, size_t tamanho);
void Inserir(HashTable *ht, char *id, double x, double y);
void Remover(HashTable *ht, char *id);
bool GetCoords(HashTable *ht, char *id, double *x, double *y);

#endif // HASH_TABLE_H
