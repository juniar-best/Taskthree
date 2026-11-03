//
// Created by Juniar on 2026/11/3.
//

#include "oled.h"
#include "App.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static void OLED_Refresh(void)
{
    Sys_t   s;
    uint8_t stepLen;

    xSemaphoreTake(SysLock, portMAX_DELAY);
    s = Sys;
    xSemaphoreGive(SysLock);

    stepLen = (s.Step >= 1000) ? 4 : 3;

    OLED_Clear();

    OLED_ShowString(1, 1, "STATE:");
    OLED_ShowString(1, 7, (s.State == State_Run) ? "RUN" : "PAUSE");

    OLED_ShowString(2, 1, "DIR:");
    OLED_ShowString(2, 5, (s.Dir == Dir_Forward) ? "FWD" : "REV");

    OLED_ShowString(3, 1, "STEP:");
    OLED_ShowNum(3, 6, s.Step, stepLen);
    OLED_ShowString(3, 6 + stepLen, "ms");

    OLED_ShowString(4, 1, "LED:");
    OLED_ShowNum(4, 5, s.Idx + 1, 1);
}

void OLEDTask(void *pvParameters)
{
    (void)pvParameters;

    OLED_Init();

    OLED_Refresh();

    for (;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        OLED_Refresh();
    }
}
