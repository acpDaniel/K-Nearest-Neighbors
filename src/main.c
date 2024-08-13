#include "../include/quadTree.h"
#include "../include/heap.h"
#include "../include/hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void imprimirNosQuadTree(const QuadTree *qt)
{
    printf("Percorrendo os nós da QuadTree:\n");
    for (int i = 0; i < qt->capacidade; ++i)
    {
        const Node *node = &(qt->nodes[i]);
        if (node->hasPoint)
        {
            if (node->active)
            {
                printf("Posição %d: Ponto (%.2f, %.2f) está ativo.\n", i, node->point.x, node->point.y);
                printf("Relação de parentesco:\n");
                printf("pai: %d\n", node->parent);
                printf("nw: %d\n", node->nw);
                printf("ne: %d\n", node->ne);
                printf("sw: %d\n", node->sw);
                printf("se: %d\n\n", node->se);
            }
            else
            {
                printf("Posição %d: Ponto (%.2f, %.2f) está desativado.\n", i, node->point.x, node->point.y);
                printf("Relação de parentesco:\n");
                printf("pai: %d\n", node->parent);
                printf("nw: %d\n", node->nw);
                printf("ne: %d\n", node->ne);
                printf("sw: %d\n", node->sw);
                printf("se: %d\n\n", node->se);
            }
        }
        else
        {
            printf("Posição %d: Não há ponto.\n", i);
        }
    }
}

void replace_comma_with_dot(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (str[i] == ',')
        {
            str[i] = '.';
        }
    }
}

void read_csv(const char *filename, QuadTree *qt, HashTable *ht)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Erro ao abrir o arquivo CSV\n");
        exit(EXIT_FAILURE);
    }

    char line[512];                  // Buffer para cada linha
    fgets(line, sizeof(line), file); // Pular a primeira linha (cabeçalho)

    while (fgets(line, sizeof(line), file))
    {
        addr_t point;
        char *token;

        // Processamento de cada campo
        token = strtok(line, ";");
        if (token)
        {
            point.idend = strdup(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            point.id_logrado = atol(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            point.sigla_tipo = strdup(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            point.nome_logra = strdup(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            point.numero_imo = atoi(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            point.nome_bairr = strdup(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            point.nome_regio = strdup(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            point.cep = atoi(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            replace_comma_with_dot(token);
            point.x = atof(token);
        }

        token = strtok(NULL, ";");
        if (token)
        {
            replace_comma_with_dot(token);
            point.y = atof(token);
        }

        // Inserir o ponto na QuadTree
        insert(qt, point);

        // Inserir o ponto na tabela hash
        Inserir(ht, point.idend, point.x, point.y);
    }

    fclose(file);
}

void processarGeracarga(const char *filename, QuadTree *qt, HashTable *ht)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo geracarga.ev");
        return;
    }

    char line[128];
    fgets(line, sizeof(line), file);

    int num_commands = atoi(line); // Lê o número de comandos
    while (fgets(line, sizeof(line), file) && num_commands > 0)
    {
        char command;
        sscanf(line, "%c", &command);

        if (command == 'A')
        {
            char id[256];
            sscanf(line + 1, "%s", id);

            double x = 0, y = 0;
            GetCoords(ht, id, &x, &y);
            activate(qt, id, x, y);
        }
        else if (command == 'D')
        {
            char id[256];
            sscanf(line + 1, "%s", id);

            double x = 0, y = 0;
            GetCoords(ht, id, &x, &y);
            deactivate(qt, id, x, y);
        }
        else if (command == 'C')
        {
            double x = 0, y = 0;
            int k;
            sscanf(line + 1, "%lf %lf %d", &x, &y, &k);

            MaxHeap *fp = criaMaxHeap(k); // Cria a fila de prioridade para os k pontos mais próximos
            Ponto p = criaPonto(x, y);
            buscaKNNRecursivoOtimizado(qt, 0, &p, k, fp);

            for (int i = 0; i < size(fp); ++i)
            {
                DistanciaPonto dp = getItem(fp, i);
                printf("%s %s, %d, %s, %s, %d", dp.ponto->sigla_tipo,
                       dp.ponto->nome_logra, dp.ponto->numero_imo,
                       dp.ponto->nome_bairr, dp.ponto->nome_regio,
                       dp.ponto->cep);
                printf(" (%f)\n", dp.distancia);
            }

            free(fp);
        }
        num_commands--;
    }

    fclose(file);
}

int main()
{
    // Abrindo o arquivo 'outra.base' para ler o tamanho da QuadTree e HashTable
    FILE *file = fopen("outra.base", "r");
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo 'outra.base'");
        return 1;
    }

    int tamanho;
    fscanf(file, "%d", &tamanho); // Lê o primeiro número do arquivo
    fclose(file);

    // Cria a QuadTree e a HashTable com o tamanho lido
    QuadTree *qt = criaQuadTree(tamanho);
    HashTable ht = criaHashTable(tamanho);

    // Lê o arquivo 'teste.csv' como antes
    read_csv("teste.csv", qt, &ht);
    // imprimirNosQuadTree(qt);

    // Chama a função para processar o arquivo 'geracarga.ev'
    processarGeracarga("geracarga.ev", qt, &ht);

    return 0;
}

int Oldmain()
{
    QuadTree *qt = criaQuadTree(10); // Capacidade inicial de 10 nós

    HashTable ht = criaHashTable(10);

    double x = 0;
    double y = 0;

    read_csv("teste.csv", qt, &ht);
    GetCoords(&ht, "05330600093F", &x, &y);
    // deactivate(qt, "12885800280", x, y);
    //   deactivate(qt, "12885800280", 617442.377411602, 7797332.07665223);

    imprimirNosQuadTree(qt);

    // int K = 4;
    // MaxHeap *fp = criaMaxHeap(K); // Cria a fila de prioridade para os 4 pontos mais próximos

    // // Executa a busca dos k pontos mais próximos
    // Ponto p = criaPonto(3.0, 4.0);
    // buscaKNN(qt, &p, K, fp);

    // printf("Os 4 pontos mais próximos a (3.0, 4.0):\n");
    // for (int i = 0; i < size(fp); ++i)
    // {
    //     DistanciaPonto dp = getItem(fp, i);
    //     printf("%s %s, %d, %s, %s, %d", dp.ponto->sigla_tipo,
    //            dp.ponto->nome_logra, dp.ponto->numero_imo,
    //            dp.ponto->nome_bairr, dp.ponto->nome_regio,
    //            dp.ponto->cep);
    //     printf(" (%f)\n", dp.distancia);
    //     // printf("(%.2f, %.2f) distância: %.2f\n", dp.ponto->x, dp.ponto->y, dp.distancia);
    // }

    // MaxHeap *fp2 = criaMaxHeap(K); // Cria a fila de prioridade para os 4 pontos mais próximos
    // buscaKNNRecursivoOtimizado(qt, 0, &p, K, fp2);

    // printf("Os 4 pontos mais próximos a (3.0, 4.0):\n");
    // for (int i = 0; i < size(fp2); ++i)
    // {
    //     DistanciaPonto dp = getItem(fp2, i);
    //     printf("%s %s, %d, %s, %s, %d", dp.ponto->sigla_tipo,
    //            dp.ponto->nome_logra, dp.ponto->numero_imo,
    //            dp.ponto->nome_bairr, dp.ponto->nome_regio,
    //            dp.ponto->cep);
    //     printf(" (%f)\n", dp.distancia);
    //     // printf("(%.2f, %.2f) distância: %.2f\n", dp.ponto->x, dp.ponto->y, dp.distancia);
    // }

    // destroiMaxHeap(fp);
    // destroiQuadTree(qt);

    return 0;
}