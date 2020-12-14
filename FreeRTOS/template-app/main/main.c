/*
Arquivo: main.c
Projeto 1: Implementar uma contagem de produtos atraves de 3 esteiras, utilizando o sistema operacional FreeRTOS.  
Desenvolvedores:
    Lucas Jose da Cunha
    Luiz Alberto Zimmermmann Zabel Martins Pinto
    Teddy Ordoñez Nuñez

Professor: Felipe Viel
Disciplina: Sistemas em Tempo Real 
Universidade:Universidade do Vale do Itajai - UNIVALI
*/

#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/touch_pad.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_timer.h"

#define TOUCH_THRESH_NO_USE (0)
#define TOUCH_THRESH_PERCENT (80)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

static bool s_pad_activated[TOUCH_PAD_MAX];
static uint32_t s_pad_init_val[TOUCH_PAD_MAX];

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

// Numero de produtos
#define PRODUCTS_SIZE 150

static int nProducts = 0;
static float weights[PRODUCTS_SIZE] = {0x0};

// Variaveis de manipulação das tasks
TaskHandle_t xHandle_display = NULL;
TaskHandle_t xHandles[3];

// Semaforo
SemaphoreHandle_t Semaphore = NULL;
SemaphoreHandle_t Semaphore_Sum = NULL;

// Timers
int64_t totalStart, sumStart, sumEnd; 

void weightSum()
{
    // Bloqueia o acesso ao recurso compartilhado para as outras tasks
    if (xSemaphoreTake(Semaphore_Sum, (TickType_t)0))
    {

        float totalWeight = 0;

        for (int i = 0; i < PRODUCTS_SIZE; i++)
        {
            totalWeight += weights[i];
        }

        printf("Peso TOTAL: %f \n", totalWeight);

        if (xSemaphoreGive(Semaphore_Sum) != pdTRUE)
        {
            printf("Erro ao liberar o semaforo!!!");
        }
    }
}

void productSum(float weight)
{

    // Bloqueia o acesso ao recurso compartilhado para as outras tasks
    if (xSemaphoreTake(Semaphore, (TickType_t)0))
    {
        weights[nProducts] = weight;
        nProducts++;
        if (nProducts >= PRODUCTS_SIZE)
        {
            sumStart = esp_timer_get_time();
            weightSum();
            nProducts = 0;
            sumEnd = esp_timer_get_time();
            printf("Tempo contagem: %fs\n", ((double) (sumEnd - sumStart)) / 1000000);
            printf("Tempo execução: %fs\n", ((double) (sumEnd - totalStart)) / 1000000);
            totalStart = esp_timer_get_time();
        }

        if (xSemaphoreGive(Semaphore) != pdTRUE)
        {
            printf("Erro ao liberar o semaforo!!!");
        }
    } // fim semaforo
}

void conveyorBelt_A(void *pvParameter)
{
    TickType_t xLastWakeTime;


    // Retorna o tempo atual
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        // Espera o produto para somar
        vTaskDelayUntil(&xLastWakeTime, CONVEYOR_T1 / portTICK_RATE_MS);
        productSum(WEIGHT_C1);
    }
}

void conveyorBelt_B(void *pvParameter)
{
    TickType_t xLastWakeTime;

    // Retorna o tempo atual
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        // Espera o produto para somar
        vTaskDelayUntil(&xLastWakeTime, CONVEYOR_T2 / portTICK_RATE_MS);
        productSum(WEIGHT_C2);
    }
}

void conveyorBelt_C(void *pvParameter)
{
    TickType_t xLastWakeTime;

    // Retorna o tempo atual
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        // Espera o produto para somar
        vTaskDelayUntil(&xLastWakeTime, CONVEYOR_T3 / portTICK_RATE_MS);
        productSum(WEIGHT_C3);
    }
}

void display(void *pvParameter)
{

    TickType_t xLastWakeTime;

    // Retorna o tempo atual
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, UPDATE_TIME / portTICK_RATE_MS);
        printf("Nº de Produtos: %d\n", nProducts);
    }
}

static void touchPad(void *pvParameter)
{
    while (1)
    {
        // Habilita interrupção touch
        touch_pad_intr_enable();
        for (int i = 0; i < TOUCH_PAD_MAX; i++)
        {
            if (s_pad_activated[i] == true)
            {
                s_pad_activated[i] = false;
                if (i == 0 || i == 3) // Lado esquerdo
                {
                    printf("Ocorreu uma interrupção!\n");
                    for (int x = 0; x < 3; x++)
                    {
                        vTaskSuspend(xHandles[x]);
                        printf("Task %d suspensa!\n", x);
                    }
                    vTaskSuspend(xHandle_display);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }else // Lado direito
                {
                    printf("Reiniciando...\n");
                    for (int x = 0; x < 3; x++)
                    {
                        vTaskResume(xHandles[x]);
                        printf("Task %d resumida...\n", x);
                    }
                    vTaskResume(xHandle_display);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
            } 
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void tp_example_set_thresholds(void)
{
    uint16_t touch_value;
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
    {
        // Lê o valor filtrado
        touch_pad_read_filtered(i, &touch_value);
        s_pad_init_val[i] = touch_value;
        // Define a interrupção do threshold.
        ESP_ERROR_CHECK(touch_pad_set_thresh(i, touch_value * 2 / 3));
    }
}

static void tp_example_rtc_intr(void *arg)
{
    uint32_t pad_intr = touch_pad_get_status();
    touch_pad_clear_status();
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
    {
        if ((pad_intr >> i) & 0x01)
        {
            s_pad_activated[i] = true;
        }
    }
}

static void tp_example_touch_pad_init(void)
{
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
    {
        touch_pad_config(i, TOUCH_THRESH_NO_USE);
    }
}

void app_main()
{
    nvs_flash_init();
    // Inicializa periférico do botão de interrupção (touch_pad)
    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    tp_example_touch_pad_init();
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    tp_example_set_thresholds();
    touch_pad_isr_register(tp_example_rtc_intr, NULL);

    // Semaforo
    vSemaphoreCreateBinary(Semaphore);
    vSemaphoreCreateBinary(Semaphore_Sum);
    // Inicializa as tasks do programa
    // Criacao das threads xTaskCreate(codigoTask, nome, tam_Task, NULL, nivelPrioridade, NULL)
    xTaskCreate(&touchPad, "touchPad", 2048, NULL, 2, NULL);
    totalStart = esp_timer_get_time();
    xTaskCreate(&conveyorBelt_A, "conveyorBelt_A", 2048, NULL, 2, &xHandles[0]);
    xTaskCreate(&conveyorBelt_B, "conveyorBelt_B", 2048, NULL, 2, &xHandles[1]);
    xTaskCreate(&conveyorBelt_C, "conveyorBelt_C", 2048, NULL, 2, &xHandles[2]);
    xTaskCreate(&display, "display", 2048, NULL, 2, &xHandle_display);
}
