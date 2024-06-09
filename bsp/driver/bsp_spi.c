#include "bsp_spi.h"

void spi_polling_xfer_byte(spi_dev_t *dev, uint8_t *datas, uint32_t len)
{
    if (!dev || !datas || 0 == len)
        return;

    uint16_t xfer_len = 0;

    // setup SPI is transmitter
    dev->spix->CR1 |= SPI_CR1_HDDIR;

    while (len > 0) {
        if (len < 0xFFFF)
            xfer_len = len;
        else
            xfer_len = 0xFFFF;

        // wirte data size to Tx register TSIZE
        dev->spix->CR2 = xfer_len;

        // enable SPI
        dev->spix->CR1 |= SPI_CR1_SPE;

        // start SPI master transfer
        dev->spix->CR1 |= SPI_CR1_CSTART;

        // start transfer spi data
        for (uint16_t i = 0; i < xfer_len; ++i) {
            while ((dev->spix->SR & SPI_FLAG_TXP) == RESET)
                ;
            *((__IO uint8_t *)&dev->spix->TXDR) = *((const uint8_t *)(datas + i));
        }

        len -= xfer_len;

        // Wait transfer done
        while ((dev->spix->SR & SPI_SR_EOT) == RESET);
        dev->spix->IFCR |= (SPI_IFCR_TXTFC | SPI_IFCR_EOTC);

        // Disable SPI transfer
        dev->spix->CR1 &= ~SPI_CR1_SPE;
    }
}

void spi_polling_xfer_halfword(spi_dev_t *dev, uint16_t *datas, uint32_t len)
{
    if (!dev || !datas || 0 == len)
        return;

    uint16_t xfer_len = 0;

    // setup SPI transfer data size 16bits
    MODIFY_REG(dev->spix->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_16BIT);

    // setup SPI is transmitter
    dev->spix->CR1 |= SPI_CR1_HDDIR;

    while (len > 0) {
        if (len < 0xFFFF)
            xfer_len = len;
        else
            xfer_len = 0xFFFF;

        // wirte data size to Tx register TSIZE
        dev->spix->CR2 = xfer_len;

        // enable SPI
        dev->spix->CR1 |= SPI_CR1_SPE;

        // start SPI master transfer
        dev->spix->CR1 |= SPI_CR1_CSTART;

        // start transfer spi data
        for (uint16_t i = 0; i < xfer_len; ++i) {
            while ((dev->spix->SR & SPI_FLAG_TXP) == RESET)
                ;
            *((__IO uint16_t *)&dev->spix->TXDR) = *((const uint16_t *)(datas + i));
        }

        len -= xfer_len;

        // Wait transfer done
        while ((dev->spix->SR & SPI_SR_EOT) == RESET);
        dev->spix->IFCR |= (SPI_IFCR_TXTFC | SPI_IFCR_EOTC);

        // Disable SPI transfer
        dev->spix->CR1 &= ~SPI_CR1_SPE;
    }
    // Reset SPI transfer data size to 8bits
    MODIFY_REG(dev->spix->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_8BIT);
}
