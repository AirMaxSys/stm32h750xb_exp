#include <stdio.h>
#include "stm32h7xx_ll_usart.h"

#define DBG_UART        UART4

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
    int cnt = 0;
    (void)fd;

    while (len--) {
        LL_USART_TransmitData8(DBG_UART, *ptr++);
        while (!LL_USART_IsActiveFlag_TC(DBG_UART));
        cnt++;
    }
    return cnt;
}

int _read(int fd, char *ptr, int len)
{
    int cnt = 0;
    (void)fd;

    while (len--) {
        while (!LL_USART_IsActiveFlag_RXNE(DBG_UART));
        *ptr++ = LL_USART_ReceiveData8(DBG_UART);
        cnt++;
    }
    return cnt;
}
#else
int fputc(int ch, FILE *f)
{
    (void)f;
    LL_USART_TransmitData8(DBG_UART, ch);
    while (!LL_USART_IsActiveFlag_TC(DBG_UART));
    return ch;
}
#endif
