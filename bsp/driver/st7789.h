#ifndef _ST7789_H_
#define _ST7789_H_

#include "stm32h750xx.h"
#include "stm32h7xx_hal.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_RST_UNSELECT()    (LCD_RST_GPIO_Port->BSRR = (LCD_CS_Pin))
#define LCD_RST_SELECT()      (LCD_RST_GPIO_Port->BSRR = (LCD_CS_Pin << 16))
#define LCD_CS_UNSELECT()     (LCD_CS_GPIO_Port->BSRR = (LCD_CS_Pin))
#define LCD_CS_SELECT()       (LCD_CS_GPIO_Port->BSRR = (LCD_CS_Pin << 16))
#define LCD_PWR_ON()          (LCD_PWR_GPIO_Port->BSRR = (LCD_PWR_Pin))
#define LCD_PWR_OFF()         (LCD_PWR_GPIO_Port->BSRR = (LCD_PWR_Pin << 16))
#define LCD_DAT()             (LCD_A0_GPIO_Port->BSRR = (LCD_A0_Pin))
#define LCD_CMD()             (LCD_A0_GPIO_Port->BSRR = (LCD_A0_Pin << 16))

void st7789_setup(void);
void st7789_draw(void);

#ifdef __cplusplus
}
#endif

#endif
