#ifndef __SDIO_H__
#define __SDIO_H__

#include <stdint.h>
#include "stm32h750xx.h" // sdmmc structure

#ifdef  __cplusplus
extern "C" {
#endif

void mcu_sdio_init(SDMMC_TypeDef *sdmmc);
uint8_t mcu_sdio_cmd_no_resp(SDMMC_TypeDef *sdmmc, uint8_t cmd, uint32_t arg);
uint8_t mcu_sdio_cmd_with_resp(SDMMC_TypeDef *sdmmc, uint8_t cmd, uint8_t arg, uint32_t *resp);

#ifdef  __cplusplus
}
#endif

#endif
