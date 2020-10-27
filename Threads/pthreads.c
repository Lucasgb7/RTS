/*  ---- COMANDOS PARA EXECUÇÃO ----
    gcc pthreads.c -o pthreads -lpthread
    ./pthreads
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define LENGTH 1500 

// funcao para executar a segunda thread
void *inc_x(void *x_void_ptr){
    // incrementa ate 100
    int *x_ptr = (int *)x_void_ptr;
    while (++(*x_ptr) < 100)
        ;

    printf("x incremento completo\n");

    // a funcao deve retornar algo, senão irá retornar NULL
    return NULL;
}

int main() {
    int x = 0, y = 0;
    printf("x: %d, y: %d\n", x, y); // printa os valores iniciais
    pthread_t inc_x_thread; // referencia para segunda thread
    // cria segunda thread que executa a funcao inc_x
    if (pthread_create(&inc_x_thread, NULL, inc_x, &x)){
        fprintf(stderr, "Erro ao criar thread\n");
        return 1;
    }
    // incrementa y até 100 na primeira thread
    while (++y < 100)
        ;

    printf("y incremento completo\n");

    // espera a segunda thread terminar
    if (pthread_join(inc_x_thread, NULL)){
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    // mostra o resultado
    printf("x: %d, y: %d\n", x, y);

    return 0;
}