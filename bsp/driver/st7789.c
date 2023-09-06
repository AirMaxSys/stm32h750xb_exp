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

void st7789_init(void)
{

}
