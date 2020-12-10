/*
Arquivo: main.c
Projeto 1: Implementar uma contagem de produtos atraves de 3 esteiras, utilizando o sistema operacional FreeRTOS.  
Desenvolvedores:
    Lucas Jose da Cunha
    Luiz Alberto Zimmermmann Zabel Martins Pinto

Professor: Felipe Viel
Disciplina: Sistemas em Tempo Real
Universidade:Universidade do Vale do Itajai - UNIVALI
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
 
// Tempo que cada produto entra em cada esteira (milisegundos) 
#define CONVEYOR_T1 1000
#define CONVEYOR_T2 500
#define CONVEYOR_T3 100

// Tempo para atualização do display (milisegundos) 
#define UPDATE_TIME 2000

// Peso de cada produto de cada esteira (kg)
#define WEIGHT_C1 5.0
#define WEIGHT_C2 2.0
#define WEIGHT_C3 0.5


static int nProducts = 0;
static float weights[1500] = {0x0};

void weightSum()
{
    // suspender as demais tasks
    
    float totalWeight = 0;
    
    // criar threads para realizar a soma paralelamente
    for(int i = 0; i < 1500; i++)
    {
        totalWeight += weights[i];
    }

    printf("Peso TOTAL: %f \n", totalWeight);    

    // retornar as demais tasks
}

void productSum(float weight)
{
    // mutex (semaforo)
    weights[nProducts] = weight;
    nProducts++;

    if (nProducts >= 1500)
    {
        weightSum(); 
        nProducts = 0;
    }
    // end mutex
}

void conveyorBelt_A(void *pvParameter)
{    
    TickType_t xLastWakeTime;

    // tempo atual
    xLastWakeTime = xTaskGetTickCount ();

	while(1)
	{
        // aguardar produto
        vTaskDelayUntil(&xLastWakeTime, CONVEYOR_T1 / portTICK_RATE_MS);

	    // somar produto
        productSum(WEIGHT_C1);
	}
}

void conveyorBelt_B(void *pvParameter)
{
    TickType_t xLastWakeTime;

    // tempo atual
    xLastWakeTime = xTaskGetTickCount ();

    while(1)
	{
         // aguardar produto
        vTaskDelayUntil(&xLastWakeTime, CONVEYOR_T2 / portTICK_RATE_MS);

	    // somar produto
        productSum(WEIGHT_C2);
	}
}

void conveyorBelt_C(void *pvParameter)
{
    TickType_t xLastWakeTime;

    // tempo atual
    xLastWakeTime = xTaskGetTickCount ();

    while(1)
	{
         // aguardar produto
        vTaskDelayUntil(&xLastWakeTime, CONVEYOR_T3 / portTICK_RATE_MS);

	    // somar produto
        productSum(WEIGHT_C3);
	}
}


void display(void *pvParameter)
{
    
    TickType_t xLastWakeTime;

    // tempo atual
    xLastWakeTime = xTaskGetTickCount ();

    while(1) 
    {
        vTaskDelayUntil(&xLastWakeTime, UPDATE_TIME / portTICK_RATE_MS);
        printf("Nº de Produtos: %d\n", nProducts);
    }
}
 
 
void app_main()
{
    nvs_flash_init();
    // Criacao das threads xTaskCreate(codigoTask, nome, tam_Task, NULL, nivelPrioridade, NULL)
    xTaskCreate(&conveyorBelt_A, "conveyorBelt_A", 2048, NULL, 2, NULL);
    xTaskCreate(&conveyorBelt_B, "conveyorBelt_B", 2048, NULL, 2, NULL);
    xTaskCreate(&conveyorBelt_C, "conveyorBelt_C", 2048, NULL, 2, NULL);
    xTaskCreate(&display, "display", 2048, NULL, 3, NULL);
    // criar task para monitoramento do botão de parada

}