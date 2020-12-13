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
static bool turnON = NULL;

// Tempo que cada produto entra em cada esteira (milisegundos)
#define CONVEYOR_T1 1000
#define CONVEYOR_T2 500
#define CONVEYOR_T3 100

// Tempo para atualização do display (milisegundos)
#define UPDATE_TIME 2000

// Peso de cada produto de cada esteira (kg)
#define SIZE 150
#define WEIGHT_C1 5.0
#define WEIGHT_C2 2.0
#define WEIGHT_C3 0.5

static int nProducts = 0;
static float weights[SIZE] = {0x0};

// Variaveis de manipulação das tasks
TaskHandle_t xHandle_Belt_A = NULL;
TaskHandle_t xHandle_Belt_B = NULL;
TaskHandle_t xHandle_Belt_C = NULL;
TaskHandle_t xHandle_display = NULL;

// Semaforo
SemaphoreHandle_t Semaphore = NULL;

void weightSum()
{
    printf("Iniciou weightSum()\n");
    // TODO suspender as demais tasks
    vTaskSuspend(xHandle_Belt_A);
    printf("Suspend A\n");
    vTaskSuspend(xHandle_Belt_B);
     printf("Suspend B\n");
    vTaskSuspend(xHandle_Belt_C);
     printf("Suspend C\n");
    vTaskSuspend(xHandle_display);
     printf("Suspend D\n");

    printf("Passou weightSum()\n");
    float totalWeight = 0;

    for (int i = 0; i < SIZE; i++)
    {
        totalWeight += weights[i];
    }

    printf("Peso TOTAL: %f \n", totalWeight);

    // retornar as demais tasks
    vTaskResume(xHandle_Belt_A);
    vTaskResume(xHandle_Belt_B);
    vTaskResume(xHandle_Belt_C);
    vTaskResume(xHandle_display);
}

void productSum(float weight)
{
    vSemaphoreCreateBinary(Semaphore);
    //xSemaphore = xSemaphoreCreateMutex();
    // mutex (semaforo)
    if (xSemaphoreTake(Semaphore, (TickType_t)0))
    {
        // We now have the semaphore and can access the shared resource.
        // ...
        // We have finished accessing the shared resource so can free the
        // semaphore.

        weights[nProducts] = weight;
        nProducts++;
        if (nProducts >= SIZE)
        {
            weightSum();
            nProducts = 0;
        }

        if (xSemaphoreGive(Semaphore) != pdTRUE)
        {
            printf("To aqui 3");
            // We would not expect this call to fail because we must have
            // obtained the semaphore to get here.
        }
    } // end mutex
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
        printf("A chamou\n");
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
        printf("B chamou\n");
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
        printf("C chamou\n");
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
                printf("i: %d", i);
                s_pad_activated[i] = false;
                if (i == 0 || i == 3)
                {
                    printf("Ocorreu uma interrupção!\n");
                    vTaskSuspend(xHandle_Belt_A);
                    vTaskSuspend(xHandle_Belt_B);
                    vTaskSuspend(xHandle_Belt_C);
                    vTaskSuspend(xHandle_display);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
                else
                {
                    printf("Reiniciando...\n");
                    vTaskResume(xHandle_Belt_A);
                    vTaskResume(xHandle_Belt_B);
                    vTaskResume(xHandle_Belt_C);
                    vTaskResume(xHandle_display);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void tp_example_read_task(void *pvParameter){
    
    while (1) {
        //interrupt mode, enable touch interrupt
        touch_pad_intr_enable();
        for (int i = 0; i < TOUCH_PAD_MAX; i++) {
            if (s_pad_activated[i] == true) {
                i++;
                printf("\nUsuario pediu para abrir a porta: %d\n", i);
                vTaskDelay(10 / portTICK_PERIOD_MS);
                // Clear information on pad activation
                s_pad_activated[i] = false;
            }
        }
        vTaskDelay(1000/ portTICK_PERIOD_MS);

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
    //clear interrupt
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
        //init RTC IO and mode for touch pad.
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

    xTaskCreate(&touchPad, "touchPad", 2048, NULL, 2, NULL);
    // Criacao das threads xTaskCreate(codigoTask, nome, tam_Task, NULL, nivelPrioridade, NULL)
    xTaskCreate(&conveyorBelt_A, "conveyorBelt_A", 2048, NULL, 2, &xHandle_Belt_A);
    xTaskCreate(&conveyorBelt_B, "conveyorBelt_B", 2048, NULL, 2, &xHandle_Belt_B);
    xTaskCreate(&conveyorBelt_C, "conveyorBelt_C", 2048, NULL, 2, &xHandle_Belt_C);
    xTaskCreate(&display, "display", 2048, NULL, 2, &xHandle_display);
}