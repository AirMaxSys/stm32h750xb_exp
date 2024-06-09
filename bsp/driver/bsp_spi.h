#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "stm32h7xx.h"

typedef struct spi_dev {
    SPI_HandleTypeDef *spix;
} spi_dev_t;

void spi_polling_xfer_byte(uint8_t *datas, uint32_t len);
void spi_polling_xfer_halfword(uint16_t *datas, uint32_t len);

#endif