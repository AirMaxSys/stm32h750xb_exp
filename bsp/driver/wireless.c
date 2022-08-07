#include <stdio.h>
#include <stdint.h>
#include "sdio.h"

#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_sdmmc.h"
#include "stm32h7xx_ll_utils.h"

#define WL_HOST_ON()     LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_3)
#define WL_HOST_OFF()    LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_3)

#define WL_REG_ON()      LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13)
#define WL_REG_OFF()     LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13)

static SDMMC_TypeDef *wl_sdio = SDMMC2;

void hw_wl_init_test(void)
{
    uint32_t data = 0, err;
    uint8_t cnt = 0;

    WL_REG_ON();
    WL_HOST_ON();

    LL_mDelay(10);

    mcu_sdio_init(wl_sdio);

    err = SDMMC_CmdGoIdleState(wl_sdio);
    printf("err:0x%08lx\r\n", err);
    mcu_sdio_cmd_no_resp(wl_sdio, 0x05, 0);
    err = SDMMC_CmdSetRelAdd(wl_sdio, (uint16_t *)&data);
    printf("CMD3 RCA:0x%08lx err:0x%08lx\r\n", data, err);

#if 0
    do {
        // cmd0
        mcu_sdio_cmd_no_resp(wl_sdio, 0x00, 0);
        // cmd5
        mcu_sdio_cmd_no_resp(wl_sdio, 0x05, 0);
        // cmd3 get RCA
        // mcu_sdio_cmd_with_resp(wl_sdio, 0x03, 0, &data);
        err = SDMMC_CmdSetRelAdd(wl_sdio, (uint16_t *)&data);
        // SDMMC_CmdSetRelAdd(wl_sdio, &data);
        printf("CMD3 RCA:0x%08lx err:0x%08lx\r\n", data, err);
        LL_mDelay(10);
    } while (cnt++ < 5);
#endif
    printf("SDMMC PWR state:%d\n", SDMMC_GetPowerState(wl_sdio));
}
