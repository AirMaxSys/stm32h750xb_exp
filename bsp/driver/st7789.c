/**********************************************************************************
 * ST7789 displayer board using SPI half-duplex serial bus.
 *  - SPI bus maximum speed: 70Mhz (single period about 14ns)
 *  - communication 4-lines interface I: CS CLK SDA A0(D/CX:data or command)
 *  - other wires: VCC(3.3V) GND RESET(Keep low level longer than 10us) LED(3.3V)
 *  - RGB/BGR format: 444(12bits) 565(16bits) 666(18bits)
 *  - display resolution: 240x320 tft resolution 128*160
**********************************************************************************/

#include "st7789.h"

// ST7789 commands definition
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
#define VSCRDEF     0x33    // Vertical scrolling definition
#define TEOFF       0x34    // Tearing effect line off
#define TEON        0x35    // Tearing effect line on
#define MADCTL      0x36    // Memory data access control
#define VSCRSADD    0x37    // Vertical scroll start address of RAM
#define IDMOFF      0x38    // Idle mode off
#define IDMON       0x39    // Idle mode on
#define COLMOD      0x3A    // Interface pixel format
#define RAMWRC      0x3C    // Write memory continue
#define RAMRDC      0x3E    // Read memory continue
#define TESCAN      0x44    // Set tear scanline
#define RDTESCAN    0x45    // Get scanline
#define WRDISBV     0x51    // Write display brightness
#define RDDISBV     0x52    // Read display brightness value
#define WRCTRLD     0x53    // Write CTRL display
#define RDCTRLD     0x54    // Read CTRL display
#define WRCACE      0x55    // Write content adaptive brightness control and color enhancement
#define RDCACB      0x56    // Read content adaptive brightness control
#define WRCABCMB    0x5E    // Write CABC minimum brightness
#define RDCABCMB    0x5F    // Read CABC minimum brightness
#define RDABCSDR    0x68    // Read automatic brightness control self-diagnostic result
#define RDID1       0xDA    // Read ID1
#define RDID2       0xDB    // Read ID2
#define RDID3       0xDC    // Read ID3
#define RAMCTRL     0xB0    // RAM control
#define RGBCTRL     0xB1    // RGB control
#define PORCTRL     0xB2    // Porch control
#define FRCTRL1     0xB3    // Frame rate control 1
#define PARCTRL     0xB5    // Partial control
#define GCTRL       0xB7    // Gate control
#define GTADJ       0xB8    // Gate on timing adjustment
#define DGMEN       0xBA    // Digital gamma enable
#define VCOMS       0xBB    // VCOM setting
#define POWSAVE     0xBC    // Power saving mode

#define IDSET       0xC1    // ID conde setting


extern SPI_HandleTypeDef hspi2;

static void st7789_hw_reset(void)
{
    LCD_RST_SELECT();
    HAL_Delay(1);
    LCD_RST_UNSELECT();
}

static void st7789_write_cmd(uint8_t cmd)
{
    LCD_CMD();
    LCD_CS_SELECT();
    HAL_SPI_Transmit(&hspi2, &cmd, 1, 0xFFFF);
    LCD_CS_UNSELECT();
}

static void st7789_write_data(uint8_t *data, uint16_t data_size)
{
    LCD_CS_SELECT();
    LCD_DAT();
    HAL_SPI_Transmit(&hspi2, data, data_size, 0xFFFF);
    LCD_CS_UNSELECT();
}

// @brief wirte command with or without parameters
static void st7789_write_para(uint8_t cmd, uint8_t *para, uint16_t para_size)
{
    LCD_CS_SELECT();
    LCD_CMD();
    HAL_SPI_Transmit(&hspi2, &cmd, 1, 0xFFFF);
    LCD_DAT();
    HAL_SPI_Transmit(&hspi2, para, para_size, 0xFFFF);
    LCD_CS_UNSELECT();
}


// @brief read command with or without parameters
static void st7789_read_para(uint8_t cmd, uint8_t *para, uint16_t para_size)
{
    LCD_CS_SELECT();
    LCD_CMD();
    HAL_SPI_Transmit(&hspi2, &cmd, 1, 0xFFFF);
    LCD_DAT();
    HAL_SPI_Receive(&hspi2, para, para_size, 0xFFFF);
    LCD_CS_UNSELECT();
}

void st7789_setup(void)
{
    uint8_t para = 0x0;
    uint8_t datas[128] = {0x0};
    // Hardware reset first
    // software reset
    st7789_write_cmd(SWRESET);
    
#if 1
    datas[0] = 0x74;
    st7789_write_para(MADCTL, datas, 1);

    datas[0] = 0x85;
    datas[1] = 0x85;
    datas[2] = 0x5A;
    st7789_write_para(IDSET, datas, 3);

    st7789_read_para(RDID1, datas, 2);
    printf("RDID1:0x%02x 0x%02x\n", datas[0], datas[1]);
    st7789_read_para(RDID2, datas, 2);
    printf("RDID2:0x%02x 0x%02x\n", datas[0], datas[1]);
    st7789_read_para(RDID2, datas, 2);
    printf("RDID3:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7789_read_para(RDDID, datas, 4);
    // st7789_write_cmd(RDDID);
    // LCD_DAT();
    // LCD_CS_SELECT();
    // HAL_SPI_Receive(&hspi2, datas, 4, 0xFFFF);
    // LCD_CS_UNSELECT();
    printf("RDID:0x%02x 0x%02x 0x%02x 0x%02x\n", datas[0], datas[1], datas[2], datas[3]);

    // st7789_read_para(RDDST, datas, 4);
    // printf("RDDST:0x%02x 0x%02x 0x%02x 0x%02x\n", datas[0], datas[1], datas[2], datas[3]);

    st7789_read_para(RDDPM, datas, 2);
    printf("RDDPM:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7789_read_para(RDDMADCTL, datas, 2);
    printf("RDDMADCTL:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7789_read_para(COLMOD, datas, 2);
    printf("COLMOD:0x%02x 0x%02x\n", datas[0], datas[1]);
    para = 0x63;
    st7789_write_para(COLMOD, &para, 1);

    st7789_read_para(RDDCOLMOD, datas, 2);
    printf("RDDCOLMOD:0x%02x 0x%02x\n", datas[0], datas[1]);

    st7789_read_para(RDDIM, datas, 2);
    printf("RDDIM:0x%02x 0x%02x\n", datas[0], datas[1]);
#endif

    st7789_hw_reset();
    HAL_Delay(100);

    st7789_write_cmd(INVON);

    st7789_write_cmd(SLPOUT);
    LCD_PWR_ON();
    st7789_write_cmd(DISPON);

    // Set drawing window
    datas[0] = 0x0;
    datas[1] = 0x0;
    datas[2] = 0x0;
    datas[3] = 128;
    // st7789_write_para(CASET, datas, 4);
    st7789_write_cmd(CASET);
    for (uint8_t i = 0; i < 4; ++i)
        st7789_write_data(datas, 4);
    datas[0] = 0x0;
    datas[1] = 0x0;
    datas[2] = 0x0;
    datas[3] = 160;
    // st7789_write_para(RASET, datas, 4);
    st7789_write_cmd(RASET);
    for (uint8_t i = 0; i < 4; ++i)
        st7789_write_data(datas, 4);

    // Set RGB666
    para = 0x65;
    st7789_write_cmd(COLMOD);
    st7789_write_data(&para, 1);

    st7789_read_para(RDDCOLMOD, datas, 2);
    printf("RDDCOLMOD:0x%02x 0x%02x\n", datas[0], datas[1]);

}

uint8_t pixels[128*160*4] = {0x0};

void spi_tans_fast(uint8_t *datas, uint32_t len)
{
    for (uint32_t i = 0; i < len; ++i) {
	// SPI2->CFG1 = SPI_BAUDRATEPRESCALER_8 | 7;
    // SPI2->CR1 = SPI_CR1_SSI;
    SPI2->CR2 = 1;
    // SPI2->CR1 = SPI_CR1_SPE | SPI_CR1_SSI;
    SPI2->CR1 = SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_CSTART;

        while ((SPI2->SR & SPI_FLAG_TXE) == 0);

        *((__IO uint8_t *)&SPI2->TXDR) = datas[i];
        
        while ((SPI2->SR & SPI_SR_TXC) == 0);
        
        SPI2->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;
	SPI2->CR1 &= ~(SPI_CR1_SPE);
    }

}

void st7789_draw(void)
{
    uint8_t para = 0x0;
    uint8_t datas[128] = {0x0};
    static uint8_t loop = 0;
    uint32_t len = 128*160*16;

#if 1
    // Trasnfer
    #if 0

    for (uint32_t i = 0; i < len>>1; ++i) {
        if (0 == loop) {
            pixels[2*i] = 0x1F;
            pixels[2*i+1] = 0x0;
        } else if (1 == loop) {
            pixels[2*i] = 0xE0;
            pixels[2*i+1] = 0x7;
        } else {
            pixels[2*i] = 0x0;
            pixels[2*i+1] = 0xF8;
        }
    }
    #else
    if (0 == loop)
        memset(pixels, 0x1F, len>>2);
    else if (1 == loop)
        memset(pixels, 0xF0, len>>2);
    else
        memset(pixels, 0x0A, len>>2);
    #endif
    printf("loop:%d\n", loop);
    if (!(++loop % 3)) loop = 0;

    // for (uint8_t i = 0; i < 10; ++i)
    //     printf("%d: 0x%02x 0x%02x\n", len-i, pixels[len - 2*i - 1], pixels[len - 2*i-2]);

    st7789_write_cmd(RAMWR);

    LCD_CS_SELECT();
    LCD_DAT();

    // while (len > 0) {
    //     if (len >= 0xFFFF) {
    //         HAL_SPI_Transmit(&hspi2, pixels, 0xFFFF, 0xFFFF);
    //         len -= 0xFFFF;
    //     } else {
    //         HAL_SPI_Transmit(&hspi2, pixels, len, 0xFFFF);
    //         len = 0;
    //     }
    // }
    spi_tans_fast(pixels, len);
    LCD_CS_UNSELECT();
#endif
}
