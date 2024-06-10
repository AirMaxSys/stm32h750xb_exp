#include "bsp_spi.h"

#define SPI_XFER_BYTE       1
#define SPI_XFER_HARFWORD   2
#define SPI_XFER_WORD       3

/**
 * @note  Function type refer to fwrite()
 * @brief SPI transmit data from master to slave in polling mode
 * @param dev SPI device struct pointer
 * @param tbuf Data trasmisstion buffer
 * @param size Each data frame size  - @ref SPI_XFER_
 * @param nmemb Number of data frmae
 */
void spi_trans_polling(spi_dev_t *dev, void *tbuf, size_t size, size_t nmemb)
{
    if (!dev || !tbuf || 0 == nmemb || 0 == size)
        return;

    // setup SPI transfer data frame size
    if (SPI_XFER_BYTE == size)
        MODIFY_REG(dev->spix->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_8BIT);
    else if (SPI_XFER_HARFWORD == size)
        MODIFY_REG(dev->spix->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_16BIT);
    else if (SPI_XFER_WORD == size)
        MODIFY_REG(dev->spix->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_32BIT);
    else
        return;

    // setup SPI is transmitter
    dev->spix->CR1 |= SPI_CR1_HDDIR;

    // enable SPI
    dev->spix->CR1 |= SPI_CR1_SPE;

    // start SPI master transfer
    dev->spix->CR1 |= SPI_CR1_CSTART;

    // start transfer spi data
    for (size_t i = 0; i < nmemb; ++i) {
        while ((dev->spix->SR & SPI_FLAG_TXP) == RESET)
            ;
        dev->spix->TXDR = *(volatile uint32_t *)tbuf;
    }

    // In endless transfer data status, SPI need to be suspended
    // dev->spix->CR1 |= SPI_CR1_CSUSP;

    // stop SPI master transfer
    dev->spix->CR1 &= ~SPI_CR1_SPE;
}

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
