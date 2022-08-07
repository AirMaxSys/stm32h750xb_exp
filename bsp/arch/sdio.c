#include "stm32h7xx_ll_gpio.h"  // gpio
#include "stm32h7xx_ll_bus.h"   // rcc
#include "stm32h7xx_ll_sdmmc.h" // sdmmc
#include "stm32h7xx_ll_utils.h" // delay


/**
 * @brief sdmmc1 gpios initializtion
 * @note ART-Pi board sdmmc1 bus use to sd memory card
 */
static void mcu_sdmmc1_bus_gpio_init(void)
{
    // sdmmc1
    //    clock      -> pc12
    //    command    -> pd2
    //    datas[0:3] -> pc[8:11]
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

    LL_GPIO_InitTypeDef gpio_structure = {0};

    gpio_structure.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12;
    gpio_structure.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_structure.Pull = LL_GPIO_PULL_UP;
    gpio_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_structure.Alternate = LL_GPIO_AF_12;   // stm32h750xb datasheet
    gpio_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    LL_GPIO_Init(GPIOC, &gpio_structure);

    gpio_structure.Pin = LL_GPIO_PIN_2;
    gpio_structure.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_structure.Pull = LL_GPIO_PULL_UP;
    gpio_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_structure.Alternate = LL_GPIO_AF_12;   
    gpio_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    LL_GPIO_Init(GPIOD, &gpio_structure);
}

/**
 * @brief sdmmc2 gpios initializtion
 * @note ART-Pi board sdmmc2 bus use to wireless chip
 */
static void mcu_sdmmc2_bus_gpio_init(void)
{
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
    gpio_structure.Alternate = LL_GPIO_AF_9;    // stm32h750xb datasheet
    gpio_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    LL_GPIO_Init(GPIOB, &gpio_structure);

    gpio_structure.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    gpio_structure.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_structure.Pull = LL_GPIO_PULL_UP;
    gpio_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_structure.Alternate = LL_GPIO_AF_11;    // stm32h750xb datasheet
    gpio_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    LL_GPIO_Init(GPIOD, &gpio_structure);
}

void mcu_sdio_init(SDMMC_TypeDef *sdmmc)
{
    SDMMC_InitTypeDef sdmmc_init_structure = {0};

    if (sdmmc == SDMMC1) {
        LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_SDMMC1);
        mcu_sdmmc1_bus_gpio_init();
    } else if (sdmmc == SDMMC2) {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_SDMMC2);
        mcu_sdmmc2_bus_gpio_init();
    } else {
        return;
    }

    sdmmc_init_structure.ClockDiv = SDMMC_INIT_CLK_DIV; // when sdmmc init clock speed < 400khz
    sdmmc_init_structure.BusWide = SDMMC_BUS_WIDE_1B;   // FIXME how many bus lines when sdio initialize?
    sdmmc_init_structure.ClockEdge= SDMMC_CLOCK_EDGE_RISING;
    sdmmc_init_structure.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    sdmmc_init_structure.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    SDMMC_Init(sdmmc, sdmmc_init_structure);

    SDMMC_SetSDMMCReadWaitMode(sdmmc, SDMMC_READ_WAIT_MODE_CLK);

    // SDMMC power on
    SDMMC_PowerState_ON(sdmmc);

    // Enable?
    // sdmmc->CLKCR |= (1<<8UL);

    // FIXME after power on wait first 74(185us@400khz) SDMMC_CK cycles
    LL_mDelay(1);

    // TODO dma configuration
    // TODO sdmmc interrupt configuration
}

uint8_t mcu_sdio_cmd_with_resp(SDMMC_TypeDef *sdmmc, uint8_t cmd, uint8_t arg, uint32_t *resp)
{
    uint8_t ret = 0;
    SDMMC_CmdInitTypeDef cmd_reg_structure = {0};

    cmd_reg_structure.CmdIndex = cmd;
    cmd_reg_structure.Argument = arg;
    cmd_reg_structure.CPSM = SDMMC_CPSM_ENABLE;
    cmd_reg_structure.Response = SDMMC_RESPONSE_SHORT;
    cmd_reg_structure.WaitForInterrupt = SDMMC_WAIT_NO;
    SDMMC_SendCommand(sdmmc, &cmd_reg_structure);

    // check last command response index
    if (cmd != (uint8_t)sdmmc->RESPCMD) {
        return -1;
    }

    // FIXME wait clock cycle
    // LL_mDelay(1);

    // FIXME which response register?
    // *resp = sdmmc->RESP1;
    // *resp = *(volatile uint32_t *)sdmmc->RESP2;
    // *resp = *(volatile uint32_t *)sdmmc->RESP2;

    // TODO check error flag and timeout
    return ret;
}

// FIXME LL library has defined those functions
uint8_t mcu_sdio_cmd_no_resp(SDMMC_TypeDef *sdmmc, uint8_t cmd, uint32_t arg)
{
    uint8_t ret = 0;
    SDMMC_CmdInitTypeDef cmd_reg_structure = {0};

    cmd_reg_structure.CmdIndex = cmd;
    cmd_reg_structure.Argument = arg;
    cmd_reg_structure.CPSM = SDMMC_CPSM_ENABLE;
    cmd_reg_structure.Response = SDMMC_RESPONSE_NO;
    cmd_reg_structure.WaitForInterrupt = SDMMC_WAIT_NO;
    SDMMC_SendCommand(sdmmc, &cmd_reg_structure);

    // TODO check error flag and timeout
    return (ret == 0);
}
