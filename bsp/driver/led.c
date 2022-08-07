#include "led.h"
#include "stm32h750xx.h"
#include "stm32h7xx_ll_utils.h"
#include <string.h>

void led_on(const char *led_color)
{
    if (!led_color) return;

    if (strncmp(led_color, "red", 3) == 0)
        LL_GPIO_SetOutputPin(LED_R_GPIO_Port, LED_R_Pin);
    else if (strncmp(led_color, "blue", 4) == 0)
        LL_GPIO_SetOutputPin(LED_B_GPIO_Port, LED_B_Pin);
    else
        return;
}

void led_off(const char *led_color)
{
    if (!led_color) return;

    if (strncmp(led_color, "red", 3) == 0)
        LL_GPIO_ResetOutputPin(LED_R_GPIO_Port, LED_R_Pin);
    else if (strncmp(led_color, "blue", 4) == 0)
        LL_GPIO_ResetOutputPin(LED_B_GPIO_Port, LED_B_Pin);
    else
        return;
}

void led_blink(const char *led_color, uint32_t tm)
{
    if (!led_color && 0 == tm)  return;

    led_on(led_color);
    LL_mDelay(tm);
    led_off(led_color);
    LL_mDelay(tm);
}
