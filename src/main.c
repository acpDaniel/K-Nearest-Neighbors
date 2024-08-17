#include "../include/quadTree.h"
#include "../include/heap.h"
#include "../include/hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_csv(char *filename, QuadTree *qt, HashTable *ht)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Erro ao abrir o arquivo CSV\n");
        exit(EXIT_FAILURE);
    }

    char line[512];
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file))
    {
        addr_t point;
        char *token;

        // Processamento de cada campo
        token = strtok(line, ";");
        point.idend = strdup(token);

        token = strtok(NULL, ";");
        point.id_logrado = atol(token);

        token = strtok(NULL, ";");
        point.sigla_tipo = strdup(token);

        token = strtok(NULL, ";");
        point.nome_logra = strdup(token);

        token = strtok(NULL, ";");
        point.numero_imo = atoi(token);

        token = strtok(NULL, ";");
        point.nome_bairr = strdup(token);

        token = strtok(NULL, ";");
        point.nome_regio = strdup(token);

        token = strtok(NULL, ";");
        point.cep = atoi(token);

        token = strtok(NULL, ";");
        point.x = atof(token);

        token = strtok(NULL, ";");
        point.y = atof(token);

        // Inserir o ponto na QuadTree
        insert(qt, point);

        // Inserir o ponto na tabela hash
        Inserir(ht, point.idend, point.x, point.y);
    }

    fclose(file);
}

void processarGeracarga(char *filename, QuadTree *qt, HashTable *ht)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo geracarga.ev");
        return;
    }

    char line[128];

    // Ignora a primeira linha
    fgets(line, sizeof(line), file);

    // Lê o arquivo até o final
    while (fgets(line, sizeof(line), file))
    {
        char command;
        sscanf(line, "%c", &command);
        // Imprime o comando que será executado
        printf("%s", line);

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

            MaxHeap *fp = criaMaxHeap(k);
            Ponto p = criaPonto(x, y);
            // buscaKNNRecursivo(qt, 0, &p, k, fp);
            buscaKNNRecursivoOtimizado(qt, 0, &p, k, fp);
            imprimeHeapEmOrdemCrescente(fp);
            free(fp);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    char *arquivoBase = NULL;
    char *arquivoEventos = NULL;

    // Processa os argumentos da linha de comando manualmente
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc)
        {
            arquivoBase = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-e") == 0 && i + 1 < argc)
        {
            arquivoEventos = argv[i + 1];
            i++;
        }
    }

    // Verifica se os arquivos foram fornecidos
    if (arquivoBase == NULL || arquivoEventos == NULL)
    {
        // Define os nomes dos arquivos padrão
        if (arquivoBase == NULL)
        {
            arquivoBase = "geracarga.base";
        }
        if (arquivoEventos == NULL)
        {
            arquivoEventos = "geracarga.ev";
        }
    }

    // Abrindo o arquivo de base para ler o tamanho da QuadTree e HashTable
    FILE *file = fopen(arquivoBase, "r");
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo de base");
        return 1;
    }

    int tamanho;
    fscanf(file, "%d", &tamanho);
    fclose(file);

    // Cria a QuadTree e a HashTable com o tamanho lido. Os parâmetros de max e min das coordenadas foram baseados no arquivo com todos os endereços de BH
    QuadTree *qt = criaQuadTree(tamanho, 598017.313632323, 7790968.24200244, 619122.989979841, 7804103.33566041);
    HashTable ht = criaHashTable(tamanho);

    // Lê o arquivo com as estações de recarga
    read_csv(arquivoBase, qt, &ht);

    // Chama a função para processar o arquivo de eventos
    processarGeracarga(arquivoEventos, qt, &ht);

    destroiQuadTree(qt);
    destroiHashTable(&ht);

    return 0;
}
