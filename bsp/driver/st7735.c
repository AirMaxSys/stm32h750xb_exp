/**********************************************************************************
 * lcd controller is ST7735 using SPI half-duplex serial bus to communication.
 *  - SPI bus maximum speed: Write 16.6Mhz(60ns) Read 8.3Mhz(120)
 *  - communication 4-lines interface I: CS CLK SDA A0(D/CX:data or command)
 *  - other wires: VCC(3.3V) GND RESET(Keep low level longer than 10us) LED(3.3V)
 *  - RGB format: RGB444 RGB565 RGB666
 *  - display resolution: 128*160
**********************************************************************************/

#include "st7735.h"

// st7735 commands definition
#define NOP         0x00    // No operation
#define SWRESET     0x01    // Software reset
#define RDDID       0x04    // Read display ID
#define RDDST       0x09    // Read display status
#define RDDPM       0x0A    // Read display power mode
#define RDDMADCTL   0x0B    // Read display MADCTL (Memory data access control)
#define RDDCOLMOD   0x0C    // Read display pixel format(RGB444/RGB565/RGB666)
#define RDDIM       0x0D    // Read display image mode
#define RDDSM       0x0E    // Read display signal mode
#define RDDSDR      0x0F    // Read display self-diagnostic result
#define SLPIN       0x10    // Sleep in (LCD enter the minimum power consumption mode)
#define SLPOUT      0x11    // Sleep out
#define PTLON       0x12    // Partial display mode on (drawing window described by the PTLAR<0x30>)
#define NORON       0x13    // Normal display mode on (turn off partial mode off)
#define INVOFF      0x20    // Display inversion off
#define INVON       0x21    // Display inversion on
#define GAMSET      0x26    // Gamma set
#define DISPOFF     0x28    // Display off
#define DISPON      0x29    // Display on
#define CASET       0x2A    // Column address set (When write LCD RAM need to configure X boundary of drawing windows first)
#define RASET       0x2B    // Row address set
#define RAMWR       0x2C    // Memory write
#define RAMRD       0x2E    // Memory read
#define PTLAR       0x30    // Parial Area
#define TEOFF       0x34    // Tearing effect line off
#define TEON        0x35    // Tearing effect line on
#define MADCTL      0x36    // Memory data access control
#define IDMOFF      0x38    // Idle mode off
#define IDMON       0x39    // Idle mode on
#define COLMOD      0x3A    // Interface pixel format
#define RDID1       0xDA    // Read ID1
#define RDID2       0xDB    // Read ID2
#define RDID3       0xDC    // Read ID3

#define BLUE        0x001F
#define GREEN       0x07E0
#define RED         0xF800
#define BLACK       0x0000
#define WHITE       0xFFFF

uint16_t ATTR_DMA_LOCATION pixels[128*160] = {0x0};

extern SPI_HandleTypeDef hspi2;

static void spi_polling_xfer_halfword(uint16_t *datas, uint32_t len)
{
    // setup SPI transfer data size 16bits
    MODIFY_REG(SPI2->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_16BIT);

    // setup SPI is transmitter
    SPI2->CR1 |= SPI_CR1_HDDIR;

    // setup TSIZE value
    if (len >= 0xFFFF)
        SPI2->CR2 = 0xFFFF;
    else
        SPI2->CR2 = len;

    // enable SPI
    SPI2->CR1 |= SPI_CR1_SPE;
    
    while (len > 0) {
        // start SPI master transfer
        SPI2->CR1 |= SPI_CR1_CSTART;

        if (len < 0xFFFF) {
            // wirte data size to Tx register TSIZE
            SPI2->CR2 = len;

            // start transfer spi data
            for (uint16_t i = 0; i < len; ++i) {
                while ((SPI2->SR & SPI_FLAG_TXP) == RESET)
                    ;
                *((__IO uint16_t *)&SPI2->TXDR) = *((const uint16_t *)(datas + i));
            }

            // change parameter value
            len = 0;
        } else {
            SPI2->CR2 = 0xFFFF;

            for (uint16_t i = 0; i < 0xFFFF; ++i) {
                while ((SPI2->SR & SPI_FLAG_TXP) == RESET)
                    ;
                *((__IO uint16_t *)&SPI2->TXDR) = *((const uint16_t *)(datas + i));
            }

            len -= 0xFFFF;
        }

        // Wait transfer done
        while ((SPI2->SR & SPI_SR_EOT) == RESET);
        SPI2->IFCR |= (SPI_IFCR_TXTFC | SPI_IFCR_EOTC);
    }

    // Disable SPI transfer
    SPI2->CR1 &= ~SPI_CR1_SPE;
    // Reset SPI transfer data size to 8bits
    MODIFY_REG(SPI2->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_8BIT);
}

static void spi_polling_xfer_byte(uint8_t *datas, uint32_t len)
{
    // setup SPI is transmitter
    SPI2->CR1 |= SPI_CR1_HDDIR;

    // setup TSIZE value
    if (len >= 0xFFFF)
        SPI2->CR2 = 0xFFFF;
    else
        SPI2->CR2 = len;

    // enable SPI
    SPI2->CR1 |= SPI_CR1_SPE;
    
    while (len > 0) {
        // start SPI master transfer
        SPI2->CR1 |= SPI_CR1_CSTART;

        if (len < 0xFFFF) {
            // wirte data size to Tx register TSIZE
            SPI2->CR2 = len;

            // start transfer spi data
            for (uint16_t i = 0; i < len; ++i) {
                while ((SPI2->SR & SPI_FLAG_TXP) == RESET)
                    ;
                *((__IO uint8_t *)&SPI2->TXDR) = *((const uint8_t *)(datas + i));
            }

            // change parameter value
            len = 0;
        } else {
            SPI2->CR2 = 0xFFFF;

            for (uint16_t i = 0; i < 0xFFFF; ++i) {
                while ((SPI2->SR & SPI_FLAG_TXP) == RESET)
                    ;
                *((__IO uint8_t *)&SPI2->TXDR) = *((const uint8_t *)(datas + i));
            }

            len -= 0xFFFF;
        }

        // Wait transfer done
        while ((SPI2->SR & SPI_SR_EOT) == RESET);
        SPI2->IFCR |= (SPI_IFCR_TXTFC | SPI_IFCR_EOTC);
    }

    // Disable SPI transfer
    SPI2->CR1 &= ~SPI_CR1_SPE;
}

static void st7735_dma_setup(void)
{
    // Clock enable
    __HAL_RCC_DMA1_CLK_ENABLE();

    // setup SPI DMA enable
    SPI2->CFG1 |= SPI_CFG1_TXDMAEN;

    // reset DMA CR parameters
    DMA1_Stream0->CR = 0x0;

    // DIR memory-to-peripheral (Tx)
    MODIFY_REG(DMA1_Stream0->CR, DMA_SxCR_DIR, DMA_SxCR_DIR_0);
    // CIRC disable
    // PINC disable
    // MINC enable
    DMA1_Stream0->CR |= DMA_SxCR_MINC;
    // PSIZE half-word
    MODIFY_REG(DMA1_Stream0->CR, DMA_SxCR_PSIZE, DMA_SxCR_PSIZE_0);
    // MSIZE half-word
    MODIFY_REG(DMA1_Stream0->CR, DMA_SxCR_MSIZE, DMA_SxCR_MSIZE_0);
    // PL priority level high
    // MODIFY_REG(DMA1_Stream0->CR, DMA_SxCR_PL, DMA_SxCR_PL_1);
    // DBM double-buffer mode
    //// DMA1_Stream0->CR |= DMA_SxCR_DBM;
    printf("DMA CR:0x%08lx\n", DMA1_Stream0->CR);

    // configuration peripheral address
    DMA1_Stream0->PAR = (uint32_t)&(SPI2->TXDR);

    // setup DMAMUX - using DMAMUX1
    // configuration DMAREQ_ID in DMAMUX_CxCR
    // DMAMUX request number SPI2_RX_DMA(39) SPI2_TX_DMA(40)
    MODIFY_REG(DMAMUX1_Channel0->CCR, DMAMUX_CxCR_DMAREQ_ID, 40);
    //// DMAMUX1->CCR &= ~DMAMUX_CxCR_DMAREQ_ID;
    //// DMAMUX1->CCR |= 40;

    // setup DMAMUX DMA requests generator channel
    // DMAMUX1_RequestGenerator0->RGCR |= DMAMUX_RGxCR_GE;
    printf("DMAMUX1 CCR:0x%08lx 0x%08lx\n", DMAMUX1_Channel0->CCR, DMAMUX1_RequestGenStatus->RGSR);

    printf("SPI CR1:0x%08lx SR:0x%08lx\n", SPI2->CR1, SPI2->SR);
    printf("DMA FCR:0x%08lx\n", DMA1_Stream0->FCR);
}

// @param len [in] the number of element in buffer
static void spi_dma_xfer_halfword(uint16_t *buf, uint32_t len)
{
    // setup SPI transfer data size 16bits
    MODIFY_REG(SPI2->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_16BIT);

    // setup SPI is transmitter
    // SPI2->CR1 |= SPI_CR1_HDDIR;

    while (len > 0) {
        // Disable DMA
        DMA1_Stream0->CR &= ~DMA_SxCR_EN;
        // Setup DMA buffer address
        DMA1_Stream0->M0AR = (uint32_t)buf;

        if (len < 0xFFFF) {
            SPI2->CR2 = len;
            DMA1_Stream0->NDTR = len;
            len = 0;
        } else {
            SPI2->CR2 = 0xFFFF;
            DMA1_Stream0->NDTR = 0xFFFF;
            len -= 0xFFFF;
            buf += 0xFFFF;
        }

        // Enable DMA
        DMA1_Stream0->CR |= DMA_SxCR_EN;

        // enable SPI
        SPI2->CR1 |= SPI_CR1_SPE;

        // start SPI master transfer
        SPI2->CR1 |= SPI_CR1_CSTART;

        // Wait transmit done
        while (!(DMA1->LISR & DMA_LISR_TCIF0) || !(SPI2->SR & SPI_IFCR_EOTC))
            ;
        DMA1->LIFCR |= DMA_LIFCR_CTCIF0;

        // Disable SPI
        SPI2->CR2 &= ~SPI_CR1_SPE;
    }
    // Reset SPI transfer data size to 8bits
    MODIFY_REG(SPI2->CFG1, SPI_CFG1_DSIZE, SPI_DATASIZE_8BIT);
}

static void st7735_hw_reset(void)
{
    LCD_RST_EN();
    HAL_Delay(1);
    LCD_RST_DIS();
}

static void st7735_write_cmd(uint8_t cmd)
{
    LCD_CS_SELECT();
    LCD_CMD();
    spi_polling_xfer_byte(&cmd, 1);
    // HAL_SPI_Transmit(&hspi2, &cmd, 1, 0xFFFF);
    LCD_CS_UNSELECT();
}

static void st7735_write_data(uint8_t *datas, uint16_t size)
{
    LCD_CS_SELECT();
    LCD_DAT();
    // HAL_SPI_Transmit(&hspi2, datas, size, 0xFFFF);
    spi_polling_xfer_byte(datas, size);
    LCD_CS_UNSELECT();
}

// @brief wirte command with or without parameters
static void st7735_write_para(uint8_t cmd, uint8_t *para, uint16_t size)
{
    LCD_CS_SELECT();
    LCD_CMD();
    spi_polling_xfer_byte(&cmd, 1);
    LCD_DAT();
    spi_polling_xfer_byte(para, size);
    LCD_CS_UNSELECT();
}


// @brief read command with or without parameters
static void st7735_read_para(uint8_t cmd, uint8_t *para, uint16_t para_size)
{
    LCD_CS_SELECT();
    LCD_CMD();
    // HAL_SPI_Transmit(&hspi2, &cmd, 1, 0xFFFF);
    spi_polling_xfer_byte(&cmd, 1);
    LCD_DAT();
    // HAL_SPI_Receive(&hspi2, para, para_size, 0xFFFF);
    // TODO: Receive data
    LCD_CS_UNSELECT();
}

void st7735_setup(void)
{
    uint8_t para = 0x0;
    uint8_t datas[128] = {0x0};

    st7735_dma_setup();

    st7735_hw_reset();
    HAL_Delay(10);
    st7735_write_cmd(SWRESET);
    HAL_Delay(10);
#if 0
    st7735_read_para(RDID1, datas, 2);
    printf("RDID1:0x%02x 0x%02x\n", datas[0], datas[1]);
    st7735_read_para(RDID2, datas, 2);
    printf("RDID2:0x%02x 0x%02x\n", datas[0], datas[1]);
    st7735_read_para(RDID2, datas, 2);
    printf("RDID3:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7735_read_para(RDDID, datas, 4);
    printf("RDID:0x%02x 0x%02x 0x%02x 0x%02x\n", datas[0], datas[1], datas[2], datas[3]);

    st7735_read_para(RDDPM, datas, 2);
    printf("RDDPM:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7735_read_para(RDDMADCTL, datas, 2);
    printf("RDDMADCTL:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7735_read_para(RDDCOLMOD, datas, 2);
    printf("RDDCOLMOD:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7735_read_para(RDDIM, datas, 2);
    printf("RDDIM:0x%02x 0x%02x\n", datas[0], datas[1]);
#endif
    st7735_write_cmd(SLPOUT);
    LCD_PWR_ON();
    st7735_write_cmd(DISPON);
#if 1
    // Set drawing window
    datas[0] = 0x0;
    datas[1] = 0x0;
    datas[2] = 0x0;
    datas[3] = 128-1;
    st7735_write_para(CASET, datas, 4);

    datas[0] = 0x0;
    datas[1] = 0x0;
    datas[2] = 0x0;
    datas[3] = 160-1;
    st7735_write_para(RASET, datas, 4);

    // Set RGB565
    para = 0x65;
    st7735_write_cmd(COLMOD);
    st7735_write_data(&para, 1);
#if 0
    st7735_read_para(RDDCOLMOD, datas, 2);
    printf("RDDCOLMOD:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7735_read_para(RDDST, datas, 5);
    puts("RDDST:");
    for (uint8_t i = 0; i < 5; ++i)
        printf("0x%02x ", datas[i]);
    puts("");
#endif
#endif
}

void st7735_draw(void)
{
    static uint8_t loop = 0;
    uint16_t data = 0xFFFF;
    uint32_t len = 128*160;

    if (0 == loop)
        data = RED;
    else if (1 == loop)
        data = GREEN;
    else if (2 == loop)
        data = BLUE;
    else if (3 == loop)
        data = WHITE;
    else
        data = BLACK;

    for (uint32_t i = 0; i < len; ++i)
        pixels[i] = data;

    if (!(++loop % 5)) loop = 0;

    st7735_write_cmd(RAMWR);

    LCD_CS_SELECT();
    LCD_DAT();
    // spi_polling_xfer_halfword(pixels, len);
    spi_dma_xfer_halfword(pixels, len>>4);
    LCD_CS_UNSELECT();
}
