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

int sum;
int total_itens;
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

void *conveyorBelt_A(void *args){

    while(total_itens<LENGTH){
        //----Inserir if gettecla == tecla aqui-----//      
        pthread_mutex_lock(&lock); //Bloqueia a variavel 
        total_itens++;             //Atualiza o numero de itens
        weights[total_itens] = 1;   //Adiciona o peso do item no vetor
        pthread_mutex_unlock(&lock); //Libera a variavel 
        //-----------------------------------------//
    }

    return NULL;
}

void *conveyorBelt_B(void *args){

    while(total_itens<LENGTH){
        //----Inserir if gettecla == tecla aqui-----//
        pthread_mutex_lock(&lock); //Bloqueia a variavel 
        total_itens++;             //Atualiza o numero de itens
        weights[total_itens] = 1;   //Adiciona o peso do item no vetor
        pthread_mutex_unlock(&lock); //Libera a variavel 
        //-----------------------------------------//
    }
    return NULL;
}

void *conveyorBelt_C(void *args){

    while(total_itens<LENGTH){
        //----Inserir if gettecla == tecla aqui-----//
        pthread_mutex_lock(&lock); //Bloqueia a variavel 
        total_itens++;             //Atualiza o numero de itens
        weights[total_itens] = 1;   //Adiciona o peso do item no vetor
        pthread_mutex_unlock(&lock); //Libera a variavel 
        //-----------------------------------------//

    }
    
    return NULL;
}



int main() {

    //Inicializa as variaveis
    sum = 0
    total_itens = 0

    // Variaveis para calculo de tempo
    clock_t start, end;
    double cpu_time_used;
    // double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // Inicializa esteiras com 0 produtos
    printf("Contagem inicial\n"); 
    printf("Esteira A: %d\nEsteira B: %d\nEsteira C: %d\n", a, b, c);
    // Rerencia para as threads das esteiras
    pthread_t thread1,thread2, thread3;

    // Cria as threads das esteiras
    if (pthread_create(&thread1, NULL, *conveyorBelt_A, &)){
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

    printf("\n\n");
    while (!kbhit())
        printf("1 - Add na Esteira A\n");
        printf("2 - Add na Esteira B\n");
        printf("3 - Add na Esteira C\n");
        printf("ESC - SAIR \n");
        puts("Pressione uma tecla!");
        if((int)(getchar()) == 27){
            printf("");
        }

    
    
    //----Verificar(if) se alguma thread acabou, se sim, é porque o limite de itens foi atingido ------//
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
    //------------------------------------------------------------------------------------------------//
    
    //Soma os pesos que estão armazenados no vetor
    for(int i = 0; i<LENGTH; i++){
        sum+= weights[i];
    }

    //Mostra a soma dos pesos e a quantidade de itens processados
    printf("Peso total dos %d itens: %d", total_itens, sum);



    return 0;
}