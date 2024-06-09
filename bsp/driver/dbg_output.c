#include "stm32h7xx.h"
#include "stdio.h"

#define DBG_UART        UART4
extern UART_HandleTypeDef huart4;

// GNUC libc output/input function redirection 
#if defined (__GNUC__)
void dbg_output_setup(void)
{
    // setup stdin and stdout unbufferd
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int _write(int fd, char *ptr, int len)
{
    (void)fd;

    HAL_UART_Transmit(&huart4, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}

int _read(int fd, char *ptr, int len)
{
    (void)fd;

    HAL_UART_Receive(&huart4, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}
#else
int fputc(int ch, FILE *f)
{
    (void)f;
    HAL_UART_Transmit(&huart4, &ch, 1, 0xFFFF);
    return ch;
}
#endif
