#ifndef __SDIO_H__
#define __SDIO_H__

#include <stdint.h>
#include "stm32h7xx.h" // sdmmc structure

#ifdef  __cplusplus
extern "C" {
#endif

void hw_sdmmc_init(SDMMC_TypeDef *sdmmc);
uint32_t hw_sdio_transfer(SDMMC_TypeDef *SDMMCx, uint8_t cmd,  uint32_t argument, uint32_t resp_type);

#ifdef  __cplusplus
}
#endif

#endif
