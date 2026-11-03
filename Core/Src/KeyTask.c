//
// Created by Juniar on 2026/11/3.
//

#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "App.h"
#include "KeyTask.h"
#include "Uart.h"

#define KEY_NUM     2

typedef struct
{
    GPIO_TypeDef *Port;
    uint16_t      Pin;
} Key_t;

static const Key_t Keys[KEY_NUM] =
{
    { KEY1_GPIO_Port, KEY1_Pin },
    { KEY2_GPIO_Port, KEY2_Pin },
};

void KeyTask(void *pvParameters)
{
    GPIO_PinState previous[KEY_NUM];
    uint8_t       i;

    (void)pvParameters;

    for (i = 0; i < KEY_NUM; i++)
    {
        previous[i] = HAL_GPIO_ReadPin(Keys[i].Port, Keys[i].Pin);
    }

    for (;;)
    {
        for (i = 0; i < KEY_NUM; i++)
        {
            GPIO_PinState current = HAL_GPIO_ReadPin(Keys[i].Port, Keys[i].Pin);

            if (previous[i] == GPIO_PIN_RESET && current == GPIO_PIN_SET)
            {
                if (i == 0)
                {
                    xSemaphoreTake(SysLock, portMAX_DELAY);
                    Sys.State = (Sys.State == State_Run) ? State_Stop : State_Run;
                    xSemaphoreGive(SysLock);

                    Uart_SendFeedback();
                }
                else if (i == 1)
                {
                    xSemaphoreTake(SysLock, portMAX_DELAY);
                    Sys.Dir = (Sys.Dir == Dir_Forward) ? Dir_Backward : Dir_Forward;
                    xSemaphoreGive(SysLock);

                    Uart_SendFeedback();
                }
            }

            previous[i] = current;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
