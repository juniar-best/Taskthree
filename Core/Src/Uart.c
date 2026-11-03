//
// Created by Juniar on 2026/11/3.
//

#include "Uart.h"
#include "main.h"
#include "usart.h"
#include "App.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <stdio.h>

extern UART_HandleTypeDef huart1;

typedef struct
{
    uint8_t Head;
    uint8_t Run;
    uint8_t Dir;
    uint8_t StepL;
    uint8_t StepH;
    uint8_t Check;
} Packet_t;

static QueueHandle_t rxQueue = NULL;
static uint8_t rxByte = 0;
static char feedbackBuf[64];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        xQueueSendFromISR(rxQueue, &rxByte, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        HAL_UART_Receive_IT(&huart1, &rxByte, 1);
    }
}

void Uart_Init(void)
{
    rxQueue = xQueueCreate(64, sizeof(uint8_t));
    HAL_UART_Receive_IT(&huart1, &rxByte, 1);
}

static void Uart_HandlePacket(Packet_t *pkt)
{
    uint16_t step;

    if (pkt->Check != (uint8_t)(0xAA + pkt->Run + pkt->Dir + pkt->StepL + pkt->StepH))
    {
        return;
    }

    step = pkt->StepL | ((uint16_t)pkt->StepH << 8);

    if (step < 100 || step > 2000)
    {
        return;
    }

    xSemaphoreTake(SysLock, portMAX_DELAY);
    Sys.State = (pkt->Run == 0) ? State_Stop : State_Run;
    Sys.Dir   = (pkt->Dir == 0) ? Dir_Forward : Dir_Backward;
    Sys.Step  = step;
    xSemaphoreGive(SysLock);

    Uart_SendFeedback();
}

void UartTask(void *pvParameters)
{
    Packet_t pkt;
    uint8_t  stage = 0;
    uint8_t  b;

    (void)pvParameters;

    for (;;)
    {
        if (xQueueReceive(rxQueue, &b, portMAX_DELAY) != pdPASS)
        {
            continue;
        }

        switch (stage)
        {
            case 0:
                if (b == 0xAA)
                {
                    pkt.Head = b;
                    stage = 1;
                }
                break;

            case 1:
                pkt.Run = b;
                stage = 2;
                break;

            case 2:
                pkt.Dir = b;
                stage = 3;
                break;

            case 3:
                pkt.StepL = b;
                stage = 4;
                break;

            case 4:
                pkt.StepH = b;
                stage = 5;
                break;

            case 5:
                pkt.Check = b;
                stage = 0;
                Uart_HandlePacket(&pkt);
                break;
        }
    }
}

void Uart_SendFeedback(void)
{
    Sys_t s;
    int   len;

    xSemaphoreTake(SysLock, portMAX_DELAY);
    s = Sys;
    xSemaphoreGive(SysLock);

    len = snprintf(feedbackBuf, sizeof(feedbackBuf),
                   "STATE=%s,DIR=%s,STEP=%u,LED=%u\r\n",
                   (s.State == State_Run) ? "RUN" : "PAUSE",
                   (s.Dir == Dir_Forward) ? "FWD" : "REV",
                   (unsigned)s.Step,
                   (unsigned)(s.Idx + 1));

    if (len > 0 && len < (int)sizeof(feedbackBuf))
    {
        HAL_UART_Transmit_IT(&huart1, (uint8_t *)feedbackBuf, len);
    }
}
