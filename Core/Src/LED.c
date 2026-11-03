//
// Created by Juniar on 2026/11/3.
//

#include "LED.h"
#include "App.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void LED_Init(void)
{
    LED_ALL_OFF();
}

void LED_Show(uint8_t Num)
{
    LED_ALL_OFF();

    switch (Num)
    {
        case 1:
        LED1_ON();
        break;
        case 2:
        LED2_ON();
        break;
        case 3:
        LED3_ON();
        break;
    }
}

void LEDTask(void *pvParameters)
{
    uint8_t i;

    (void)pvParameters;

    LED_ALL_ON();
    vTaskDelay(pdMS_TO_TICKS(250));
    LED_ALL_OFF();
    vTaskDelay(pdMS_TO_TICKS(250));

    for (i = 1; i <= LED_NUM; i++)
    {
        LED_Show(i);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    LED_Show(0);
    vTaskDelay(pdMS_TO_TICKS(200));

    for (;;)
    {
        uint16_t step;
        uint8_t  num;

        xSemaphoreTake(SysLock, portMAX_DELAY);
        step = Sys.Step;
        xSemaphoreGive(SysLock);

        vTaskDelay(pdMS_TO_TICKS(step));

        xSemaphoreTake(SysLock, portMAX_DELAY);

        if (Sys.State == State_Run)
        {
            if (Sys.Dir == Dir_Backward)
            {
                Sys.Idx = (Sys.Idx == 0) ? (LED_NUM - 1) : (Sys.Idx - 1);
            }
            else
            {
                Sys.Idx = (Sys.Idx == LED_NUM - 1) ? 0 : (Sys.Idx + 1);
            }
        }
        num = Sys.Idx + 1;

        xSemaphoreGive(SysLock);

        LED_Show(num);

        if (OledTaskHandle != NULL)
        {
            xTaskNotifyGive(OledTaskHandle);
        }
    }
}
