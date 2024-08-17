#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para criar e inicializar a HashTable
HashTable criaHashTable(int tamanhoOriginal)
{
    HashTable ht;
    ht.tamanhoOriginal = tamanhoOriginal * 2;
    ht.tamanhoTabela = tamanhoOriginal * 2; // Pode começar com o tamanho original
    ht.tamanhoConjunto = 0;
    ht.tabela = (ElementoTabela *)malloc(ht.tamanhoTabela * sizeof(ElementoTabela));

    // Inicializa cada posição da tabela
    for (int i = 0; i < ht.tamanhoTabela; i++)
    {
        ht.tabela[i].id = NULL; // Inicializa com NULL ao invés de string vazia
        ht.tabela[i].x = 0;
        ht.tabela[i].y = 0;
        ht.tabela[i].vazio = true;
        ht.tabela[i].retirada = false;
    }

    return ht;
}

// Função auxiliar para dar resize na tabela
void Resize(HashTable *ht, size_t tamanho)
{
    ElementoTabela *tabelaAux = ht->tabela;
    int tamanhoAntigo = ht->tamanhoOriginal;

    ht->tabela = (ElementoTabela *)malloc(tamanho * sizeof(ElementoTabela));
    if (ht->tabela == NULL)
    {
        exit(EXIT_FAILURE);
    }

    ht->tamanhoTabela = tamanho;
    ht->tamanhoOriginal = tamanho;
    ht->tamanhoConjunto = 0;

    // Inicializa a nova tabela
    for (size_t i = 0; i < tamanho; ++i)
    {
        ht->tabela[i].vazio = true;
        ht->tabela[i].retirada = false;
    }

    // Reinsere os elementos da tabela antiga
    for (int i = 0; i < tamanhoAntigo; ++i)
    {
        if (!tabelaAux[i].vazio && !tabelaAux[i].retirada)
        {
            Inserir(ht, tabelaAux[i].id, tabelaAux[i].x, tabelaAux[i].y);
        }
    }

    free(tabelaAux); // Libera a memória antiga
}

// Função para criar a chave de hash na tabela
int Hash(char *s, int tamanho)
{
    int hashValue = 0;
    for (size_t i = 0; i < strlen(s); i++)
    {
        hashValue += (s[i]) * (i + 1);
    }

    // Se o hashValue for negativo, convertemos para positivo
    if (hashValue < 0)
    {
        hashValue = -hashValue;
    }

    return hashValue % tamanho;
}

// Função para inserir um novo elemento na tabela hash
void Inserir(HashTable *ht, char *id, double x, double y)
{
    if (ht->tamanhoConjunto + 1 >= ht->tamanhoTabela)
    {
        Resize(ht, ht->tamanhoTabela * 2);
    }

    int pos = Hash(id, ht->tamanhoTabela);
    int posAjustada = pos;
    int i = 0;
    while ((i < ht->tamanhoTabela) && !ht->tabela[posAjustada].vazio && strcmp(ht->tabela[posAjustada].id, id) != 0)
    {
        i++;
        posAjustada = (pos + i * i) % ht->tamanhoTabela;
    }

    if (ht->tabela[posAjustada].vazio || ht->tabela[posAjustada].retirada)
    {
        ht->tabela[posAjustada].id = strdup(id);
        ht->tabela[posAjustada].x = x;
        ht->tabela[posAjustada].y = y;
        ht->tabela[posAjustada].vazio = false;
        ht->tabela[posAjustada].retirada = false;
        ht->tamanhoConjunto++;
    }
}

// Função para remover um elemento da tabela
void Remover(HashTable *ht, char *id)
{
    int pos = Hash(id, ht->tamanhoTabela);
    int posAjustada = pos;
    int i = 0;

    while ((i < ht->tamanhoTabela) && !ht->tabela[posAjustada].vazio)
    {
        // Se encontrar o elemento com o ID correspondente
        if (strcmp(ht->tabela[posAjustada].id, id) == 0 && !ht->tabela[posAjustada].retirada)
        {
            // Marcar como removido
            ht->tabela[posAjustada].retirada = true;
            ht->tamanhoConjunto--;
            return;
        }

        i++;
        posAjustada = (pos + i * i) % ht->tamanhoTabela;
    }
}

// Função que recebe o id de uma estação e altera os parametros x e y por referencia para retornar as coordenadas
bool GetCoords(HashTable *ht, char *id, double *x, double *y)
{
    int pos = Hash(id, ht->tamanhoTabela);
    int posAjustada = pos;
    int i = 0;
    while ((i < ht->tamanhoTabela) && strcmp(ht->tabela[posAjustada].id, id) != 0 && !ht->tabela[posAjustada].vazio)
    {
        i++;
        posAjustada = (pos + i * i) % ht->tamanhoTabela;
    }

    if (strcmp(ht->tabela[posAjustada].id, id) == 0 && !ht->tabela[posAjustada].retirada)
    {
        *x = ht->tabela[posAjustada].x;
        *y = ht->tabela[posAjustada].y;
        return true;
    }
    return false;
}

// Função para destroir hashTable
void destroiHashTable(HashTable *ht)
{
    if (ht == NULL)
        return;

    // Itera sobre cada elemento da tabela
    for (int i = 0; i < ht->tamanhoTabela; i++)
    {
        if (!ht->tabela[i].vazio && ht->tabela[i].id != NULL)
        {
            // Libera a memória alocada para o id
            free(ht->tabela[i].id);
        }
    }

    // Libera a memória alocada para o array de elementos
    free(ht->tabela);
}
