/******************************************************************************************
 * 
 * W25Q128FV - ManufactureID: 0xEF Device ID(SPI mode):0x4018h
 *****************************************************************************************/
#include "stm32h7xx.h"
#include "bsp_spi.h"

#define W25Q_MANUFACTURE_ID         0xEF
#define W25Q_128FV_SPI_MODE_DEV_ID  0x4018

#define W25Q_RMDI       0x90    // Read manufacture/device ID
#define W25Q_RMDI_DI    0x92    // Read manufacture/device ID dual IO
#define W25Q_RMDI_QI    0x94    // Read manufacture/device ID quad IO

#define W25Q_RSR1       0x04    // Read status register-1 S7~S0
#define W25Q_RSR2       0x35    // Read status register-2 S15~S8
#define W25Q_RSR3       0x15    // Read status register-3 S23~S16

typedef enum {
    W25Q_BUS_STANDARD,
    W25Q_BUS_DUAL_SPI,
    W25Q_BUS_QUAD_SPI
} w25q_bus_enum;

spi_dev_t w25q_spi_dev = {
    .spix = SPI1,
};

int w25q_init(spi_dev_t *dev, w25q_bus_enum bus)
{
    uint32_t data = 0;

    // Serial bus init
    // Get manufacture ID and device ID
    if (bus == W25Q_BUS_STANDARD) {
        data = (W25Q_RMDI << 24);
        spi_trans_polling(dev, &data, sizeof(data), 1);
    }

    return 0;
}