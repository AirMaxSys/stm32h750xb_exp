/**********************************************************************************
 * ST7789 displayer board using SPI half-duplex serial bus.
 *  - SPI bus maximum speed: 70Mhz (single period about 14ns)
 *  - communication 4-lines interface I: CS CLK SDA A0(D/CX:data or command)
 *  - other wires: VCC(3.3V) GND RESET(Keep low level longer than 10us) LED(3.3V)
 *  - RGB/BGR format: 444(12bits) 565(16bits) 666(18bits)
 *  - display resolution: 240x320
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
#define RAWWR       0x2C    // Memory write
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


extern SPI_HandleTypeDef hspi2;

static void st7789_hw_reset(void)
{
    LCD_RST_SELECT();
    HAL_Delay(1);
    LCD_RST_UNSELECT();
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


// @brief wirte command with or without parameters
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
    

    datas[0] = 0xF8;
    st7789_write_para(MADCTL, datas, 1);
    st7789_read_para(MADCTL, &para, 1);
    printf("MADCTL:0x%02x\n", para);

    st7789_read_para(RDID1, &para, 1);
    printf("RDID1:0x%02x\n", para);
    st7789_read_para(RDID2, &para, 1);
    printf("RDID2:0x%02x\n", para);
    st7789_read_para(RDID3, &para, 1);
    printf("RDID3:0x%02x\n", para);

    st7789_read_para(RDDID, datas, 4);
    printf("RDID:0x%02x 0x%02x 0x%02x\n", datas[1], datas[2], datas[3]);
}
