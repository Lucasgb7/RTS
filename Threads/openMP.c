/*
----- Comandos para Executar -----
gcc -o openMP -fopenmp openMP.c
./openMP
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define LENGTH 10
#define MAX_WEIGHT 10

int main()
{
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Thread[%d]", id);
    }
}