/* Algoritmos e estruturas de dados III
 * Wagner Meira Jr. - 12/06/2024
 *
 * Sistema de memoria virtual
 *
 * Este codigo implementa um SMV simples, com politica
 * de gerenciamento randomica.
 *
 * A aplicacao exemplo e construcao de grafo randomico conectado,
 * o que e verificado por busca em profundidade.
 *
 * gcc -Wall -g -o segv segv3.c
 *
 */

#include "smv.h"

#define NOPROT 0    /* not protected */
#define READPROT 1  /* protected for read */
#define WRITEPROT 2 /* protected for write */
#define VALID 4     /* data in phys memory (RAM) */
#define DIRTY 8     /* data in cache, the page has been written */
#define AVAIL 16    /* data in cache, protected for R/W */
#define READ 32     /* data in cache, protected for W */
#define DISCO 64    /* data in secondary memory */

// Estrutura de página da memória virtual
typedef struct smvpage
{
    int status;
    void *logaddr;
    void *physaddr;
    int nacc;                // número de acessos
    int nvalid;              // número de vezes que se tornou válida
    int ndirty;              // número de vezes que se tornou suja
    int navail;              // número de vezes que se tornou disponível
    int nread;               // número de vezes que a leitura foi permitida
    int ndisk;               // número de vezes que foi enviada ao disco
    unsigned long last_used; // Contador para implementar LRU
} smvpage_t, *ptr_smvpage_t;

void segv_handler(int sig, siginfo_t *sip, ucontext_t *uap);

smvpage_t pvet[NUMPAGE];

char *physpage, *logpage;
int swap;                              /* file handler of swap */
int pagesinmemory = 0;                 /* número de páginas em memória */
unsigned long global_time_counter = 0; // Contador global para LRU

// Função que encontra a página menos recentemente usada (LRU)
int find_lru_page()
{
    int lru_index = -1;
    unsigned long min_last_used = ~0UL; // Inicializa com o maior valor possível

    for (int i = 0; i < NUMPAGE; i++)
    {
        if (pvet[i].status & VALID)
        {
            if (pvet[i].last_used < min_last_used)
            {
                min_last_used = pvet[i].last_used;
                lru_index = i;
            }
        }
    }
    return lru_index;
}

void segv_handler(int sig, siginfo_t *sip, ucontext_t *uap)
{
    unsigned int i = ((caddr_t)sip->si_addr - logpage) / PAGESIZE;
    int c, discard;

#ifdef DEBUG
    printf("page fault at page %d: %x\n", i, pvet[i].status);
#endif
    pvet[i].nacc++;
    global_time_counter++; // Incrementa o contador global a cada acesso

    /* Verifica se temos alguma página suja e salva se necessário */
    for (c = 0; c < NUMPAGE; c++)
    {
        if (pvet[c].status & DIRTY)
        {
            pvet[c].status &= ~DIRTY;
            pvet[c].status |= VALID;
            memcpy(pvet[c].physaddr, pvet[c].logaddr, PAGESIZE);
#ifdef DEBUG
            printf("page %d was updated: %x\n", c, pvet[c].status);
#endif
            pvet[c].nvalid++;
        }
        else if ((c != i) && (pvet[c].status & (AVAIL + READ)))
        {
            pvet[c].status &= ~(AVAIL + READ);
            pvet[c].status |= VALID;
#ifdef DEBUG
            printf("page %d became valid: %x\n", c, pvet[c].status);
#endif
            pvet[c].nvalid++;
        }
    }

    /* Protege todas as páginas a cada page fault */
    if (mprotect(logpage, NUMPAGE * PAGESIZE, PROT_NONE))
    {
        perror("Couldn't mprotect");
        exit(errno);
    }

    if (pvet[i].status & DISCO)
    {
        /* Carrega a página da memória secundária */
        if (pagesinmemory + 1 > NUMPAGE * MEMTOSWAPRATIO)
        {
            discard = find_lru_page(); // Encontra a página LRU
            if (discard != -1)
            {
                lseek(swap, discard * PAGESIZE, SEEK_SET);
                write(swap, pvet[discard].physaddr, PAGESIZE);
                pvet[discard].status &= ~VALID;
                pvet[discard].status |= DISCO;
                pvet[discard].ndisk++;
                pagesinmemory--;
#ifdef DEBUG
                printf("Discarding %d to disk (%d)\n", discard, pagesinmemory);
#endif
            }
            else
            {
                printf("ERROR: Couldn't find a page to swap out!\n");
            }
        }

        /* Carrega a nova página */
        pvet[i].status &= ~DISCO;
        pvet[i].status |= VALID;
        pvet[i].nvalid++;
        lseek(swap, i * PAGESIZE, SEEK_SET);
        read(swap, pvet[i].physaddr, PAGESIZE);
        pagesinmemory++;
        pvet[i].last_used = global_time_counter; // Atualiza last_used
#ifdef DEBUG
        printf("Loading %d to memory (%d)\n", i, pagesinmemory);
#endif
    }
    else if (pvet[i].status & VALID)
    {
        /* A página está em memória física e deve ser marcada como disponível */
        pvet[i].status &= ~VALID;
        pvet[i].status |= AVAIL;
        pvet[i].navail++;
        if (mprotect(pvet[i].logaddr, PAGESIZE, PROT_READ | PROT_WRITE))
        {
            perror("Couldn't mprotect");
            exit(errno);
        }
        memcpy(pvet[i].logaddr, pvet[i].physaddr, PAGESIZE);
        if (mprotect(pvet[i].logaddr, PAGESIZE, PROT_NONE))
        {
            perror("Couldn't mprotect");
            exit(errno);
        }
        pvet[i].last_used = global_time_counter; // Atualiza last_used
#ifdef DEBUG
        printf("page %d became avail: %x\n", i, pvet[i].status);
#endif
    }
    else if (pvet[i].status & AVAIL)
    {
        /* A página está em cache e será acessada */
        pvet[i].status &= ~AVAIL;
        pvet[i].status |= READ;
        pvet[i].nread++;
        if (mprotect(pvet[i].logaddr, PAGESIZE, PROT_READ))
        {
            perror("Couldn't mprotect");
            exit(errno);
        }
        pvet[i].last_used = global_time_counter; // Atualiza last_used
#ifdef DEBUG
        printf("page %d can be read: %x\n", i, pvet[i].status);
#endif
    }
    else if (pvet[i].status & READ)
    {
        /* A página deve ser escrita e é marcada como suja */
        pvet[i].status &= ~READ;
        pvet[i].status |= DIRTY;
        pvet[i].ndirty++;
        if (mprotect(pvet[i].logaddr, PAGESIZE, PROT_READ | PROT_WRITE))
        {
            perror("Couldn't mprotect");
            exit(errno);
        }
        pvet[i].last_used = global_time_counter; // Atualiza last_used
#ifdef DEBUG
        printf("page %d can be written: %x\n", i, pvet[i].status);
#endif
    }
    else if (pvet[i].status & DIRTY)
    {
        /* Isso nunca deve acontecer */
#ifdef DEBUG
        printf("ERROR: page %d is already dirty: %x\n", i, pvet[i].status);
#endif
    }
}

void end_page()
{
    int j;
    for (j = 0; j < NUMPAGE; j++)
    {
        if (pvet[j].nacc)
        {
            printf("Page %d: acc %d val %d drt %d ava %d rd %d dsk %d\n",
                   j, pvet[j].nacc, pvet[j].nvalid, pvet[j].ndirty,
                   pvet[j].navail, pvet[j].nread, pvet[j].ndisk);
        }
        if (pvet[j].status & VALID)
        {
            lseek(swap, j * PAGESIZE, SEEK_SET);
            write(swap, pvet[j].physaddr, PAGESIZE);
            pvet[j].status &= ~VALID;
            pvet[j].status |= DISCO;
            pagesinmemory--;
        }
    }
    close(swap);
}

char *init_page()
{
    int i;
    char swapname[30];
    char pagenull[PAGESIZE];

    // Configura o handler de sinal para tratar falhas de página
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = (void (*)(int, siginfo_t *, void *))segv_handler;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGALRM);
    sigaddset(&sa.sa_mask, SIGIO);
    sigaction(SIGSEGV, &sa, NULL);

    // Aloca espaço para as páginas físicas e lógicas
    printf("init_page(): allocating %d bytes\n", NUMPAGE * PAGESIZE + PAGESIZE - 1);
    physpage = malloc(NUMPAGE * PAGESIZE + PAGESIZE - 1);
    if (!physpage)
    {
        perror("Couldn't malloc(1024)");
        exit(errno);
    }
    logpage = malloc(NUMPAGE * PAGESIZE + PAGESIZE - 1);
    if (!logpage)
    {
        perror("Couldn't malloc(1024)");
        exit(errno);
    }

    /* Alinha para múltiplos de PAGESIZE, assumindo que é uma potência de dois */
    physpage = (char *)(((long)physpage + PAGESIZE - 1) & ~(PAGESIZE - 1));
    logpage = (char *)(((long)logpage + PAGESIZE - 1) & ~(PAGESIZE - 1));
    memset(pagenull, 0, PAGESIZE);
    sprintf(swapname, "smvswap.%d", (int)getpid());
    swap = open(swapname, O_RDWR);
    for (i = 0; i < NUMPAGE; i++)
    {
        pvet[i].status = DISCO;
        pvet[i].logaddr = logpage + i * PAGESIZE;
        pvet[i].physaddr = physpage + i * PAGESIZE;
        write(swap, pagenull, PAGESIZE);
        pvet[i].nacc = 0;      // número de vezes que foi acessada
        pvet[i].nvalid = 0;    // número de vezes que se tornou válida
        pvet[i].ndirty = 0;    // número de vezes que se tornou suja
        pvet[i].navail = 0;    // número de vezes que se tornou disponível
        pvet[i].nread = 0;     // número de vezes que a leitura foi permitida
        pvet[i].ndisk = 0;     // número de vezes que foi enviada ao disco
        pvet[i].last_used = 0; // Inicializa o contador de LRU
    }
    if (mprotect(logpage, NUMPAGE * PAGESIZE, PROT_NONE))
    {
        perror("Couldn't mprotect logpage");
        exit(errno);
    }
    return logpage;
}
