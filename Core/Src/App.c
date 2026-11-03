//
// Created by Juniar on 2026/11/3.
//

#include "App.h"

Sys_t Sys =
{
    .State = State_Run,
    .Dir   = Dir_Forward,
    .Idx   = 0,
    .Step  = 200,
};

SemaphoreHandle_t SysLock = NULL;

TaskHandle_t OledTaskHandle = NULL;

void App_Init(void)
{
    SysLock = xSemaphoreCreateMutex();
}
