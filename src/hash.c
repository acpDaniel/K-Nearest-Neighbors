#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para criar e inicializar a HashTable
HashTable criaHashTable(int tamanhoOriginal)
{
    HashTable ht;
    ht.tamanhoOriginal = tamanhoOriginal;
    ht.tamanhoTabela = tamanhoOriginal; // Pode começar com o tamanho original
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

// Função para criar a chave de hash na tabela
int Hash(char *s)
{
    int hashValue = 0;
    for (size_t i = 0; i < strlen(s); i++)
    {
        hashValue += (s[i] - '0') * (i + 1);
    }

    // Se o hashValue for negativo, convertemos para positivo
    if (hashValue < 0)
    {
        hashValue = -hashValue;
    }

    return hashValue;
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

// Função para inserir um novo elemento na tabela hash
void Inserir(HashTable *ht, char *id, double x, double y)
{
    if (ht->tamanhoConjunto + 1 >= ht->tamanhoTabela)
    {
        Resize(ht, ht->tamanhoTabela * 2);
    }

    int pos = Hash(id);
    int i = 0;
    while ((i < ht->tamanhoTabela) && !ht->tabela[(pos + i) % ht->tamanhoTabela].vazio && strcmp(ht->tabela[(pos + i) % ht->tamanhoTabela].id, id) != 0)
    {
        i++;
    }

    ht->tabela[(pos + i) % ht->tamanhoTabela].id = id;
    ht->tabela[(pos + i) % ht->tamanhoTabela].x = x;
    ht->tabela[(pos + i) % ht->tamanhoTabela].y = y;
    ht->tabela[(pos + i) % ht->tamanhoTabela].vazio = false;
    ht->tabela[(pos + i) % ht->tamanhoTabela].retirada = false;
    ht->tamanhoConjunto++;
}

// Função para remover um elemento da tabela
void Remover(HashTable *ht, char *id)
{
    int pos = Hash(id);
    int i = 0;
    while ((i < ht->tamanhoTabela) && strcmp(ht->tabela[(pos + i) % ht->tamanhoTabela].id, id) != 0 && !ht->tabela[(pos + i) % ht->tamanhoTabela].vazio)
    {
        i++;
    }

    if (strcmp(ht->tabela[(pos + i) % ht->tamanhoTabela].id, id) == 0 && !ht->tabela[(pos + i) % ht->tamanhoTabela].retirada)
    {
        ht->tabela[(pos + i) % ht->tamanhoTabela].retirada = true;
        ht->tamanhoConjunto--;
    }
}

// Função que recebe o id de uma estação e altera os parametros x e y por referencia para retornar as coordenadas
bool GetCoords(HashTable *ht, char *id, double *x, double *y)
{
    int pos = Hash(id);
    int i = 0;
    while ((i < ht->tamanhoTabela) && strcmp(ht->tabela[(pos + i) % ht->tamanhoTabela].id, id) != 0 && !ht->tabela[(pos + i) % ht->tamanhoTabela].vazio)
    {
        i++;
    }

    if (strcmp(ht->tabela[(pos + i) % ht->tamanhoTabela].id, id) == 0 && !ht->tabela[(pos + i) % ht->tamanhoTabela].retirada)
    {
        *x = ht->tabela[(pos + i) % ht->tamanhoTabela].x;
        *y = ht->tabela[(pos + i) % ht->tamanhoTabela].y;
        return true;
    }
    return false;
}

void destroiHashTable(HashTable *ht)
{
    if (ht == NULL)
        return;

    // Itera sobre cada elemento da tabela
    for (int i = 0; i < ht->tamanhoTabela; i++)
    {
        if (!ht->tabela[i].vazio && ht->tabela[i].id != NULL)
        {
            // Libera a memória alocada para a string ID, se alocada dinamicamente
            free(ht->tabela[i].id);
        }
    }

    // Libera a memória alocada para o array de elementos
    free(ht->tabela);
}
