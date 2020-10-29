/*
----- Comandos para Executar -----
gcc -o openMP -fopenmp openMP.c
./openMP
*/
#include <stdio.h>
#include <stdlib.h>     
#include <time.h>       //  Mensurar tempo
#include <unistd.h>     //  BIBLIOTECAS 
#include <termios.h>    //  PARA
#include <fcntl.h>      //  KBHIT
#include <omp.h>        //  OpenMP Library

#define LENGTH 1500       // Maximo de produtos
#define MAX_WEIGHT 10   // Peso maximo dos produtos

int total_items;
int sum_weights;
int weights[LENGTH];
int previous_items;
int input_array[LENGTH];

int input_generator()
{
    int i = rand() % 5; 
    if (i == 0){
        return 1;
    }else{
        return 0;
    }
}


int random_weight(){
    int x = rand() % MAX_WEIGHT;
    // Caso gerar um valor igual a zero, retorna 1
    // printf("\tPeso+: %d", x);
    if (x == 0)
        return 1;
    else return x;
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

int main()
{
    // Variaveis para calculo de tempo
    srand(time(NULL));
    clock_t start, end;
    double cpu_time_used;

    weights[LENGTH] = 0;
    total_items = 0;
    previous_items = 0;
#pragma omp parallel shared(total_items, previous_items, weights) num_threads(3)
    {
        printf("Thread %d iniciada.\n", omp_get_thread_num());
        while (total_items < LENGTH)
        {
            if (input_generator)
            {
#pragma omp critical // Bloqueia a variavel
                {
                    total_items += 1;                       // Atualiza o numero de itens
                    weights[total_items] = random_weight(); // Adiciona o peso do item no vetor
                }                                           // Libera a variavel
                printf("\nThread %d ativada.\n", omp_get_thread_num());
                // Atualiza display
                if (total_items != previous_items)
                {
                    update_display(total_items, update_weights(weights));
                    previous_items = total_items;
                }
            }
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
    printf("\nTempo de contagem: %f segundos", weight_time*LENGTH);
    printf("\nTempo de pesagem: %f segundos\n", weight_time);
}