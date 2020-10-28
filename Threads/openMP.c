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

#define LENGTH 10       // Maximo de produtos
#define MAX_WEIGHT 10   // Peso maximo dos produtos

int total_items;
int weights[LENGTH];
int previous_items;

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

int input_simulation()
{
    int i = (rand() % 3) + 1; // Sorteia uma das 3 esteiras para inserir
    printf("\nEsteira[%d]: ", i);
    return i;
}

int main()
{
    total_items = 0;
    previous_items = 0;
    while (1)
    {
        #pragma omp parallel shared(total_items, previous_items, weights) num_threads(3)
        {
                printf("Thread %d iniciada.\n", omp_get_thread_num());
                while (total_items < LENGTH)
                {
                    if (kbhit())
                    {
                        
                        #pragma omp critical     // Bloqueia a variavel
                        {                      
                            total_items += 100;                     // Atualiza o numero de itens
                            weights[total_items] = random_weight(); // Adiciona o peso do item no vetor
                        }                        // Libera a variavel
                        printf("Thread %d ativada.\n", omp_get_thread_num());
                        
                    }
                    // Atualiza display
                    if (total_items != previous_items){
                        update_display(total_items, update_weights(weights));
                        previous_items = total_items;
                    }
                }
            }    
        }  

        update_display(total_items, update_weights(weights));   // Printa o display

    }
    
    
}