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
#define LENGTH 10
#define MAX_WEIGHT 10

int sum_weights;
int total_items;
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
	
	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	
	return 0;
}

int random_weight(){
    int x = rand() % MAX_WEIGHT;
    // Caso gerar um valor igual a zero, retorna 1
    if (x == 0)
        return 1;
    else return x;
}

void *conveyorBelt_A(void *args)
{
    while (total_items < LENGTH)
    {
        if (kbhit())
        {   
            fflush(stdin);
            if (getchar() == '1') // Caso digite: 1
            {
                pthread_mutex_lock(&lock);   // Bloqueia a variavel
                total_items++;               // Atualiza o numero de itens
                weights[total_items] = (intptr_t) random_weight();    // Adiciona o peso do item no vetor
                pthread_mutex_unlock(&lock); // Libera a variavel

                printf("A++");
            }
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
            fflush(stdin);
            if (getchar() == '2') // Caso digite: 2
            {
                pthread_mutex_lock(&lock);   // Bloqueia a variavel
                total_items++;               // Atualiza o numero de itens
                weights[total_items] = (intptr_t) random_weight;    // Adiciona o peso do item no vetor
                pthread_mutex_unlock(&lock); // Libera a variavel

                printf("B++");
            }
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
            fflush(stdin);
            if (getchar() == '3') // Caso digite: 3
            {
                pthread_mutex_lock(&lock);              // Bloqueia a variavel
                total_items++;                          // Atualiza o numero de itens
                weights[total_items] = (intptr_t) random_weight;   // Adiciona o peso do item no vetor
                pthread_mutex_unlock(&lock);            // Libera a variavel

                printf("C++");
            }
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
    // double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC

    // Referência para as threads das esteiras
    pthread_t thread1, thread2, thread3;
    while (1)
    {
        // Inicializa as variaveis
        sum_weights = 0;
        total_items = 0;
        printf("1 - Adicionar na esteira A\n");
        printf("2 - Adicionar na esteira B\n");
        printf("3 - Adicionar na esteira C\n");
        pthread_mutex_init(&lock, NULL);

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
        pthread_mutex_destroy(&lock);
        pthread_exit(NULL);
    }
    //Soma os pesos que estão armazenados no vetor
    for (int i = 0; i < LENGTH; i++)
    {
        sum_weights += weights[i];
    }
    //Mostra a soma dos pesos e a quantidade de itens processados
    printf("Peso total dos %d itens: %d", total_items, sum_weights);

    return 0;
}