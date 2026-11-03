//
// Created by Juniar on 2026/11/3.
//

#ifndef TASKTHREE_APP_H
#define TASKTHREE_APP_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef enum
{
    State_Stop = 0,
    State_Run
} RunState_t;

typedef enum
{
    Dir_Forward = 0,
    Dir_Backward
} Dir_t;

typedef struct
{
    RunState_t State;
    Dir_t      Dir;
    uint8_t    Idx;
    uint16_t   Step;
} Sys_t;

extern Sys_t Sys;
extern SemaphoreHandle_t SysLock;
extern TaskHandle_t OledTaskHandle;

void App_Init(void);

#endif //TASKTHREE_APP_H
