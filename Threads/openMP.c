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

int main()
{
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Thread[%d]", id);
    }
}