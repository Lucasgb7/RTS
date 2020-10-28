
/*
----- Comandos para Executar -----
gcc single_thread.c -o single_thread
./single_thread
*/
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
    printf("\n-----L C D-----");
    printf("\nNº de Itens: %d", total_items);
    printf("\nPeso Total: %d kg", sum_weights);
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

int main()
{
    // Variaveis para calculo de tempo
    srand(time(NULL));
    clock_t start, end;
    double cpu_time_used;
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

        start = clock();
        while (total_items < LENGTH)
        {
            while (kbhit()){
                switch (getchar())
                {
                case '1':
                    total_items += 100;                     // Atualiza o numero de itens
                    weights[total_items] = random_weight(); // Adiciona o peso do item no vetor
                    printf("\nA++");
                    break;

                case '2':
                    total_items += 100;                     // Atualiza o numero de itens
                    weights[total_items] = random_weight(); // Adiciona o peso do item no vetor
                    printf("\nB++");
                    break;

                case '3':
                    total_items += 100;                     // Atualiza o numero de itens
                    weights[total_items] = random_weight(); // Adiciona o peso do item no vetor
                    printf("\nC++");
                    break;
                
                default:
                    break;
                }
            }
            // Atualiza display
            if (total_items != previous_items)
            {
                update_display(total_items, update_weights(weights));
                previous_items = total_items;
            }
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
        printf("\nTempo de contagem: %f segundos", weight_time * LENGTH);
        printf("\nTempo de pesagem: %f segundos\n", weight_time);
        // run--;  // Finaliza a thread
    }
    return 0;
}