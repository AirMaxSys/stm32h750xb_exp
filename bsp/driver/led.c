#include "led.h"
#include "stm32h750xx.h"
#include "stm32h7xx_ll_utils.h"
#include <string.h>

void led_on(const char *led_color)
{
    if (!led_color) return;

    if (strncmp(led_color, "red", 3) == 0)
        LED_R_GPIO_Port->BSRR = LED_R_Pin;
    else if (strncmp(led_color, "blue", 4) == 0)
        LED_B_GPIO_Port->BSRR = LED_B_Pin;
    else
        return;
}

void led_off(const char *led_color)
{
    if (!led_color) return;

    if (strncmp(led_color, "red", 3) == 0)
        LED_R_GPIO_Port->BSRR = LED_R_Pin<<16;
    else if (strncmp(led_color, "blue", 4) == 0)
        LED_B_GPIO_Port->BSRR = LED_B_Pin<<16;
    else
        return;
}

void led_blink(const char *led_color, uint32_t tm)
{
    if (!led_color && 0 == tm)  return;

    led_on(led_color);
    HAL_Delay(tm);
    led_off(led_color);
    HAL_Delay(tm);
}
