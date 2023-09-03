/*
 * AP6212(CYW43438) support WLAN and Bluetooth 5.0
 *   WLAN hanrware interface using SDIO v2.0
 *     - SDIO default speed up to 25MHz and high spped up to 50MHz
*/
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_sdmmc.h"
#include "stm32h7xx_ll_utils.h"
#include "sdio.h"
#include <stdio.h>

#define WLAN_PWR_ON()      LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13)
#define WLAN_PWR_OFF()     LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13)

#define WLAN_SUCCESS        0x00000000UL
#define WLAN_TIMEOUT        0x80000000UL

#define WLAN_ENUM_MAX_REENTRY   10U

static SDMMC_TypeDef *wl_sdio = SDMMC2;

static int sdio_recv_short(uint8_t cmd, uint32_t *data)
{
    int ret = 0;

    // Check parameter cmde
    (void)cmd;

    if (wl_sdio->STA & SDMMC_STA_CTIMEOUT) {
        printf("Line:%d SDIO Responce timeout!\r\n", __LINE__);
        return -1;
    }

    // Shell we clear ICR?

    if (data) {
        *data = wl_sdio->RESP1;
    }
    return ret;
}

static int sdio_recv_shortcrc(uint8_t cmd, uint32_t *data)
{
    int ret = 0;

    if (wl_sdio->STA & SDMMC_STA_CTIMEOUT) {
        printf("Line:%d SDIO Responce timeout!\r\n", __LINE__);
        ret = -1;
    } else if (wl_sdio->STA & SDMMC_STA_CCRCFAIL) {
        printf("Line:%d SDIO Responce CRC failed!\r\n", __LINE__);
        ret = -2;
    } else {
        uint8_t resp_cmd = (uint8_t)wl_sdio->RESPCMD & SDMMC_RESPCMD_RESPCMD;
        if (resp_cmd != cmd) {
            printf("Line:%d RESCMD=0x%02x CMD=0x%02x\r\n", __LINE__, resp_cmd, cmd);
            ret = -3;
        }
    }

    // Shell we clear ICR?

    if (data) {
        *data = wl_sdio->RESP1;
    }
    return ret;
}

static int sdio_recv_long(uint8_t cmd, uint32_t *data)
{
    int ret = 0;

    if (wl_sdio->STA & SDMMC_STA_CTIMEOUT) {
        printf("Line:%d SDIO Responce timeout!\r\n", __LINE__);
        ret = -1;
    } else if (wl_sdio->STA & SDMMC_STA_CCRCFAIL) {
        printf("Line:%d SDIO Responce CRC failed!\r\n", __LINE__);
        ret = -2;
    }

    if (data) {
        data[0] = wl_sdio->RESP1;
        data[1] = wl_sdio->RESP2;
        data[2] = wl_sdio->RESP3;
        data[3] = wl_sdio->RESP4;
    }

    return ret;
}

uint32_t wlan_sdio_enum(void)
{
    uint32_t data = 0, err;
    uint16_t cnt = 0;

    hw_sdmmc_init(wl_sdio);

    WLAN_PWR_ON();
    HAL_Delay(2);

    do {
        // The host send CMD0 to make card going to IDLE state.
        err = SDMMC_CmdGoIdleState(wl_sdio);
        if (err != SDMMC_ERROR_NONE)
            printf("SDIO go to IDLE error!\r\n");

        HAL_Delay(50);

        // IO-aware card initialize with CMD5 and the host will get the R4 response.
        err = hw_sdio_transfer(wl_sdio, 0x5U, 0x0U, SDMMC_RESPONSE_NO);
        if (err != SDMMC_ERROR_NONE)
            printf("CMD5 ERR: 0x%08lx\r\n", err);

        err = sdio_recv_short(0x05, &data);
        printf("CMD5 RESP: 0x%08lx\r\n", data);

        // Card identification mode host get RCA
        err = hw_sdio_transfer(wl_sdio, 0x3U, 0x0U, SDMMC_RESPONSE_SHORT);
        if (err != SDMMC_ERROR_NONE)
            printf("CMD3 ERR: 0x%08lx\r\n", err);

        err = sdio_recv_shortcrc(0x03, &data);
        printf("CMD3 RESP: 0x%08lx\r\n", data);

        // Send CMD7 with RCA to select the card
        err = hw_sdio_transfer(wl_sdio, 0x07, data & 0xFFFF0000U, SDMMC_RESPONSE_SHORT);
        if (SDMMC_ERROR_NONE != err)
            printf("CMD7 ERR: 0x%08lx\r\n", err);

        err = sdio_recv_shortcrc(0x07, &data);
        printf("CMD7 RESP: 0x%08lx\r\n", data);

        HAL_Delay(1);
    } while (SDMMC_ERROR_NONE != err && cnt++ < WLAN_ENUM_MAX_REENTRY);

    if (--cnt == WLAN_ENUM_MAX_REENTRY) {
        printf("ERROR: enumration timeout\r\n");
        return WLAN_TIMEOUT;
    }

    printf("RCA:0x%08lx\r\n", data);

    return WLAN_SUCCESS;
}
