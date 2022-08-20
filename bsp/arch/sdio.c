#include "stm32h7xx_ll_gpio.h"  // gpio
#include "stm32h7xx_ll_bus.h"   // rcc
#include "stm32h7xx_ll_sdmmc.h" // sdmmc
#include "stm32h7xx_ll_utils.h" // delay


#define SDIO_CLR_ALL_FLAGS(SDMMCx)  (SDMMCx)->ICR = 0xFFFFFFFFUL;
#define SDIO_ERROR_MASK             (SDMMC_STA_CCRCFAIL | SDMMC_STA_DCRCFAIL | \
                                     SDMMC_STA_CTIMEOUT | SDMMC_STA_DTIMEOUT | \
                                     SDMMC_STA_TXUNDERR | SDMMC_STA_RXOVERR)
/**
 * @brief SDMMCx GPIOs initializtion
 */
static void
hw_sdmmc_gpio_cfg(SDMMC_TypeDef *SDMMCx)
{
    LL_GPIO_InitTypeDef gpio_structure = {0};

    if (SDMMC1 == SDMMCx) {
        // sdmmc1
        //    clock      -> pc12
        //    command    -> pd2
        //    datas[0:3] -> pc[8:11]
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

        gpio_structure.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12;
        gpio_structure.Mode = LL_GPIO_MODE_ALTERNATE;
        gpio_structure.Pull = LL_GPIO_PULL_UP;
        gpio_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        gpio_structure.Alternate = LL_GPIO_AF_12;
        gpio_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        LL_GPIO_Init(GPIOC, &gpio_structure);

        gpio_structure.Pin = LL_GPIO_PIN_2;
        gpio_structure.Mode = LL_GPIO_MODE_ALTERNATE;
        gpio_structure.Pull = LL_GPIO_PULL_UP;
        gpio_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        gpio_structure.Alternate = LL_GPIO_AF_12;   
        gpio_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        LL_GPIO_Init(GPIOD, &gpio_structure);
    } else if (SDMMC2 == SDMMCx) {
        // sdmmc2
        //    clock      -> pd6
        //    command    -> pd7
        //    datas[0:3] -> pb14 pb15 pb3 pb4
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
        LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

        LL_GPIO_InitTypeDef gpio_structure = {0};

        gpio_structure.Pin = LL_GPIO_PIN_14|LL_GPIO_PIN_15|LL_GPIO_PIN_3|LL_GPIO_PIN_4;
        gpio_structure.Mode = LL_GPIO_MODE_ALTERNATE;
        gpio_structure.Pull = LL_GPIO_PULL_UP;
        gpio_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        gpio_structure.Alternate = LL_GPIO_AF_9;
        gpio_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        LL_GPIO_Init(GPIOB, &gpio_structure);

        gpio_structure.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
        gpio_structure.Mode = LL_GPIO_MODE_ALTERNATE;
        gpio_structure.Pull = LL_GPIO_PULL_UP;
        gpio_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        gpio_structure.Alternate = LL_GPIO_AF_11;
        gpio_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        LL_GPIO_Init(GPIOD, &gpio_structure);
    }
}

static void
hw_sdmmc_clk_reset(SDMMC_TypeDef *SDMMCx)
{
    if (SDMMC1 == SDMMCx) {
        LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_SDMMC1);
        LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_SDMMC1);
    } else if (SDMMC2 == SDMMCx) {
        LL_AHB2_GRP1_ForceReset(LL_AHB2_GRP1_PERIPH_SDMMC2);
        LL_AHB2_GRP1_ReleaseReset(LL_AHB2_GRP1_PERIPH_SDMMC2);
    }
}

static void
hw_sdmmc_clk_enable(SDMMC_TypeDef *SDMMCx)
{
    if (SDMMC1 == SDMMCx) {
        LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_SDMMC1);
    } else if (SDMMC2 == SDMMCx) {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_SDMMC2);
    }
}

void hw_sdmmc_init(SDMMC_TypeDef *SDMMCx)
{
    SDMMC_InitTypeDef sdmmc_init_structure = {0};

    assert_param(IS_SDMMC_ALL_INSTANCE(SDMMCx));

    SDMMC_PowerState_OFF(SDMMCx);
    hw_sdmmc_clk_reset(SDMMCx);
    SDIO_CLR_ALL_FLAGS(SDMMCx);
    hw_sdmmc_gpio_cfg(SDMMCx);
    // TODO: Enable SDMMC global interrupt

    hw_sdmmc_clk_enable(SDMMCx);
    sdmmc_init_structure.ClockDiv               = SDMMC_INIT_CLK_DIV; // when sdmmc init clock speed < 400khz
    sdmmc_init_structure.ClockEdge              = SDMMC_CLOCK_EDGE_RISING;
    sdmmc_init_structure.ClockPowerSave         = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    sdmmc_init_structure.BusWide                = SDMMC_BUS_WIDE_1B;   // FIXME how many bus lines when sdio initialize?
    sdmmc_init_structure.HardwareFlowControl    = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    SDMMC_Init(SDMMCx, sdmmc_init_structure);
    SDMMC_SetSDMMCReadWaitMode(SDMMCx, SDMMC_READ_WAIT_MODE_CLK);

    SDMMC_PowerState_ON(SDMMCx);
    // FIXME after power on wait first 74(185us@400khz) SDMMC_CK cycles
    LL_mDelay(10);

    // TODO: Config SDMMC DMA
}

#define SDIO_TRANS_MAX_REENTRY      5U

uint32_t
hw_sdio_transfer(SDMMC_TypeDef *SDMMCx, uint8_t cmd,  uint32_t argument, uint32_t *resp)
{
    uint8_t reentry = 0U;
    uint32_t temp = 0U;
    uint32_t temp_cmd = 0U;
    uint32_t count = SDMMC_CMDTIMEOUT * (SystemCoreClock/8U/1000U);

    if (!resp) {
        temp_cmd = (uint32_t) (cmd|SDMMC_RESPONSE_NO|SDMMC_WAIT_NO|SDMMC_CPSM_ENABLE);
    } else {
        temp_cmd = (uint32_t) (cmd|SDMMC_RESPONSE_SHORT|SDMMC_WAIT_NO|SDMMC_CPSM_ENABLE);
    }

begin:
    if (reentry++ >= SDIO_TRANS_MAX_REENTRY) goto exit;

    SDIO_CLR_ALL_FLAGS(SDMMCx);

    SDMMCx->ARG = argument;
    SDMMCx->CMD = temp_cmd;

    do {
        count--;
        temp = SDMMCx->STA;
        if (temp & SDIO_ERROR_MASK) goto begin;
    } while (count && (temp & SDMMC_ICR_CMDSENTC) != 0);

    if (!count) return SDMMC_ERROR_TIMEOUT;

    if (resp) {
        // check last command response index
        if (cmd != SDMMCx->RESPCMD)
            return SDMMC_ERROR_CMD_CRC_FAIL; 

        // FIXME which response register?
            *resp = SDMMCx->RESP1;
    }

exit:
    return SDMMC_ERROR_NONE;
}
