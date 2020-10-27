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

void *conveyorBelt_A(void *i){
    int *x = (int *)i;
    while(++(*x) < LENGTH)
        ;
    printf("Esteira 1 - CHEIA\t");

    return NULL;
}
void *conveyorBelt_B(void *i){
    int *x = (int *)i;
    while(++(*x) < LENGTH)
        ;
    printf("Esteira 2 - CHEIA\t");

    return NULL;
}
void *conveyorBelt_C(void *i){
    int *x = (int *)i;
    while(++(*x) < LENGTH)
        ;
    printf("Esteira 3 - CHEIA\t");

    return NULL;
}

int main() {
    int sum = 0;
    int a = 0, b = 0, c = 0;
    // Inicializa esteiras com 0 produtos
    printf("Contagem inicial\n"); 
    printf("Esteira A: %d\nEsteira B: %d\nEsteira C: %d\n", a, b, c);
    // Rerencia para as threads das esteiras
    pthread_t thread1,thread2, thread3;

    // Cria as threads das esteiras
    if (pthread_create(&thread1, NULL, *conveyorBelt_A, &a)){
        fprintf(stderr, "Erro ao criar thread\n");
        return 1;
    }
    if (pthread_create(&thread2, NULL, *conveyorBelt_B, &b)){
        fprintf(stderr, "Erro ao criar thread\n");
        return 1;
    }
    if (pthread_create(&thread3, NULL, *conveyorBelt_C, &c)){
        fprintf(stderr, "Erro ao criar thread\n");
        return 1;
    }

    sum = a + b + c;

    // Aguarda as outras threads terminarem
    if (pthread_join(thread1, NULL)){
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    if (pthread_join(thread2, NULL)){
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    if (pthread_join(thread3, NULL)){
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    // mostra o resultado
    printf("\nSoma total: %d\n", sum);
    printf("Esteira A: %d\nEsteira B: %d\nEsteira C: %d\n", a, b, c);

    return 0;
}