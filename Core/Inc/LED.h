//
// Created by Juniar on 2026/11/3.
//

#ifndef TASKTHREE_LED_H
#define TASKTHREE_LED_H

#include "main.h"

/* LED 总数：改这里就能改流水灯的位数，App.c 里所有循环都用它 */
#define LED_NUM         3

/* 高电平点亮（PA0 / PA3 / PA6 接 LED 正极，负极经电阻到 GND） */
#define LED1_ON()       HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED1_OFF()      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define LED2_ON()       HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_OFF()      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
#define LED3_ON()       HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET)
#define LED3_OFF()      HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET)

/* do{...}while(0) 把三条语句包成一个整体，
   这样 if (x) LED_ALL_ON(); 才不会只执行第一条 */
#define LED_ALL_ON()    do { LED1_ON();  LED2_ON();  LED3_ON();  } while (0)
#define LED_ALL_OFF()   do { LED1_OFF(); LED2_OFF(); LED3_OFF(); } while (0)

void LED_Init(void);

/* Num 取值：0 = 全灭，1~3 = 只亮第 Num 颗 */
void LED_Show(uint8_t Num);

void LEDTask(void *pvParameters);

#endif //TASKTHREE_LED_H
