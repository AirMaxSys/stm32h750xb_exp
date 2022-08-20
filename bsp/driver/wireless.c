#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_sdmmc.h"
#include "stm32h7xx_ll_utils.h"
#include "sdio.h"
#include "tinylog.h"

#define WLAN_PWR_ON()      LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13)
#define WLAN_PWR_OFF()     LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13)

#define WLAN_SUCCESS        0x00000000UL
#define WLAN_TIMEOUT        0x80000000UL

#define WLAN_ENUM_MAX_REENTRY   500U

static SDMMC_TypeDef *wl_sdio = SDMMC2;

uint32_t wlan_sdio_enum(void)
{
    uint32_t data = 0, err;
    uint16_t cnt = 0;

    hw_sdmmc_init(wl_sdio);

    WLAN_PWR_ON();
    LL_mDelay(2);

    do {
        SDMMC_CmdGoIdleState(wl_sdio);
        err = hw_sdio_transfer(wl_sdio, 0x5U, 0x0U, NULL);
        if (!err) printf("CMD5 ERR: 0x%08lx\r\n", err);
        err = hw_sdio_transfer(wl_sdio, 0x3U, 0x0U, &data);
        printf("%d CMD3 ERR:0x%08lx RESP:0x%08lx\r\n", cnt, err, data);
        LL_mDelay(1);
    } while (SDMMC_ERROR_NONE != err && cnt++ < WLAN_ENUM_MAX_REENTRY);
    if (--cnt == WLAN_ENUM_MAX_REENTRY)
        return WLAN_TIMEOUT;
    printf("RCA:0x%08lx\r\n", data);

    return WLAN_SUCCESS;
}
