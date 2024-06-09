#ifndef _ST7789_H_
#define _ST7789_H_

#include "stm32h7xx.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_RST_DIS()    (LCD_RST_GPIO_Port->BSRR = (LCD_CS_Pin))
#define LCD_RST_EN()      (LCD_RST_GPIO_Port->BSRR = (LCD_CS_Pin << 16))
#define LCD_CS_UNSELECT()     (LCD_CS_GPIO_Port->BSRR = (LCD_CS_Pin))
#define LCD_CS_SELECT()       (LCD_CS_GPIO_Port->BSRR = (LCD_CS_Pin << 16))
#define LCD_PWR_ON()          (LCD_PWR_GPIO_Port->BSRR = (LCD_PWR_Pin))
#define LCD_PWR_OFF()         (LCD_PWR_GPIO_Port->BSRR = (LCD_PWR_Pin << 16))
#define LCD_DAT()             (LCD_A0_GPIO_Port->BSRR = (LCD_A0_Pin))
#define LCD_CMD()             (LCD_A0_GPIO_Port->BSRR = (LCD_A0_Pin << 16))

void st7735_setup(void);
void st7735_draw(void);

#ifdef __cplusplus
}
#endif

#endif
