#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>

/**
  * 0.96 寸 OLED（SSD1306，128×64，I²C 接口）软件 I²C 驱动
  * 引脚：PB8 = SCL，PB9 = SDA（开漏输出，内部由 OLED_I2C_Init 配置）
  * 坐标：Line 取 1~4（每行 16 像素高），Column 取 1~16（每个字符 8 像素宽）
  */

void OLED_Init(void);

void OLED_Clear(void);

void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);

void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);

void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);

void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

void OLEDTask(void *pvParameters);

#endif
