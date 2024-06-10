#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "stm32h7xx.h"

typedef struct spi_dev {
    SPI_TypeDef *spix;
} spi_dev_t;

void spi_trans_polling(spi_dev_t *dev, void *tbuf, size_t size, size_t nmemb);
void spi_polling_xfer_byte(spi_dev_t *dev, uint8_t *datas, uint32_t len);
void spi_polling_xfer_halfword(spi_dev_t *dev, uint16_t *datas, uint32_t len);

#endif