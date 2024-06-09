#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LED_COLOR_RED   "red"
#define LED_COLOR_BLUE  "blue"

void led_on(const char *led_color);
void led_off(const char *led_color);
void led_blink(const char *led_color, uint32_t tm);

#ifdef __cplusplus
}
#endif

#endif