/*  ---- COMANDOS PARA EXECUÇÃO ----
    gcc pthreads.c -o pthreads -lpthread
    ./pthreads
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>

#define LENGTH 1500
#define MAX_WEIGHT 10
int sum_weights;
int total_items;
int previous_items;
int weights[LENGTH];
pthread_mutex_t lock;


int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int random_weight()
{
    int x = rand() % MAX_WEIGHT;
    // Caso gerar um valor igual a zero, retorna 1
    // printf("\tPeso+: %d", x);
    if (x == 0)
        return 1;
    else
        return x;
}

int update_weights(int weights[])
{
    // Soma os pesos que estão armazenados no vetor
    for (int i = 0; i < LENGTH; i++)
    {
        sum_weights += weights[i];
    }
    return sum_weights;
}

void update_display(int total_items, int sum_weights)
{
    printf("\n\n-----L C D-----");
    printf("\nNº de Itens: %d", total_items);
    printf("\nPeso Total: %d kg", sum_weights);
}

void *conveyorBelt_A(void *args)
{
    while (total_items < LENGTH)
    {
        if (kbhit())
        {
            if (getchar() == '1') // Caso digite: 1
            {
                pthread_mutex_lock(&lock);              // Bloqueia a variavel
                total_items += 100;                     // Atualiza o numero de itens
                weights[total_items] = random_weight(); // Adiciona o peso do item no vetor
                pthread_mutex_unlock(&lock);            // Libera a variavel

                printf("\nA++");
            }
        }
        // Atualiza display
        if (total_items != previous_items)
        {
            update_display(total_items, update_weights(weights));
            previous_items = total_items;
        }
    }

    return NULL;
}

void *conveyorBelt_B(void *args)
{
    while (total_items < LENGTH)
    {
        if (kbhit())
        {
            if (getchar() == '2') // Caso digite: 2
            {
                pthread_mutex_lock(&lock);              // Bloqueia a variavel
                total_items += 100;                     // Atualiza o numero de itens
                weights[total_items] = random_weight(); // Adiciona o peso do item no vetor
                pthread_mutex_unlock(&lock);            // Libera a variavel

                printf("\nB++");
            }
        }
        // Atualiza display
        if (total_items != previous_items)
        {
            update_display(total_items, update_weights(weights));
            previous_items = total_items;
        }
    }

    return NULL;
}

void *conveyorBelt_C(void *args)
{
    while (total_items < LENGTH)
    {
        if (kbhit())
        {
            if (getchar() == '3') // Caso digite: 3
            {
                pthread_mutex_lock(&lock);              // Bloqueia a variavel
                total_items += 100;                     // Atualiza o numero de itens
                weights[total_items] = random_weight(); // Adiciona o peso do item no vetor
                pthread_mutex_unlock(&lock);            // Libera a variavel

                printf("\nC++");
            }
        }
        // Atualiza display
        if (total_items != previous_items)
        {
            update_display(total_items, update_weights(weights));
            previous_items = total_items;
        }
    }

    return NULL;
}

int main()
{
    // Variaveis para calculo de tempo
    srand(time(NULL));
    clock_t start, end;
    double cpu_time_used;
    // Referência para as threads das esteiras
    pthread_t thread1, thread2, thread3;
    int run = 1;
    while (run)
    {
        printf("\n--- Iniciando contagem ---\n");
        // Inicializa as variaveis
        sum_weights = 0;
        total_items = 0;
        previous_items = 0;
        printf("1 - Adicionar na esteira A\n");
        printf("2 - Adicionar na esteira B\n");
        printf("3 - Adicionar na esteira C\n");
        pthread_mutex_init(&lock, NULL);
        start = clock();
        // Cria as threads das esteiras
        if (pthread_create(&thread1, NULL, *conveyorBelt_A, NULL))
        {
            fprintf(stderr, "Erro ao criar thread\n");
            return 1;
        }
        if (pthread_create(&thread2, NULL, *conveyorBelt_B, NULL))
        {
            fprintf(stderr, "Erro ao criar thread\n");
            return 1;
        }
        if (pthread_create(&thread3, NULL, *conveyorBelt_C, NULL))
        {
            fprintf(stderr, "Erro ao criar thread\n");
            return 1;
        }
        // Se alguma thread acabou, é porque o limite de itens foi atingido
        // Aguarda as outras threads terminarem
        if (pthread_join(thread1, NULL) || pthread_join(thread2, NULL) || pthread_join(thread3, NULL))
        {
            fprintf(stderr, "Erro ao aguardar a thread\n");
            return 2;
        }
        // Calcula o tempo para pesagem
        start = clock();
        update_weights(weights);
        end = clock();
        double weight_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        // Calcula a taxa de atualização do display
        start = clock();
        update_display(total_items, update_weights(weights));
        end = clock();
        double display_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        // Fornece os tempos testados
        printf("\nTaxa de atualização: %f segundos", display_time);
        printf("\nTempo de contagem: %f segundos", weight_time*LENGTH);
        printf("\nTempo de pesagem: %f segundos\n", weight_time);
        // run--;  // Finaliza a thread
    }
    return 0;
}