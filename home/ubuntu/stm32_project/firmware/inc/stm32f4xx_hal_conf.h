#ifndef STM32F4XX_HAL_CONF_H
#define STM32F4XX_HAL_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* ########################## Module Selection ############################## */
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CAN_MODULE_ENABLED
#define HAL_CRC_MODULE_ENABLED
#define HAL_CEC_MODULE_ENABLED
#define HAL_CRYP_MODULE_ENABLED
#define HAL_DAC_MODULE_ENABLED
#define HAL_DCMI_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_DMA2D_MODULE_ENABLED
#define HAL_ETH_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_NAND_MODULE_ENABLED
#define HAL_NOR_MODULE_ENABLED
#define HAL_PCCARD_MODULE_ENABLED
#define HAL_SRAM_MODULE_ENABLED
#define HAL_SDRAM_MODULE_ENABLED
#define HAL_HASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_I2S_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED
#define HAL_LTDC_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_RNG_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SAI_MODULE_ENABLED
#define HAL_SD_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED
#define HAL_IRDA_MODULE_ENABLED
#define HAL_SMARTCARD_MODULE_ENABLED
#define HAL_WWDG_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_HCD_MODULE_ENABLED

/* ########################## HSE/HSI Values adaptation ##################### */
#define HSE_VALUE    8000000U
#define HSE_STARTUP_TIMEOUT    100U
#define HSI_VALUE    16000000U

/* ########################## System Configuration ######################### */
#define VDD_VALUE                    3300U
#define TICK_INT_PRIORITY            0x0FU
#define USE_RTOS                     0U
#define PREFETCH_ENABLE              1U
#define INSTRUCTION_CACHE_ENABLE     1U
#define DATA_CACHE_ENABLE            1U

/* ########################## Assert Selection ############################## */
#define USE_FULL_ASSERT             1U

/* ################## Ethernet peripheral configuration ##################### */
#define ETH_TX_DESC_CNT         4
#define ETH_RX_DESC_CNT         4

/* ################## SPI peripheral configuration ########################## */
#define USE_SPI_CRC                 1U

/* Includes ------------------------------------------------------------------*/
/**
  * @brief Include module's header file
  */
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32f4xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32f4xx_hal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f4xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32f4xx_hal_cortex.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32f4xx_hal_adc.h"
#endif

#ifdef HAL_CAN_MODULE_ENABLED
  #include "stm32f4xx_hal_can.h"
#endif

#ifdef HAL_CRC_MODULE_ENABLED
  #include "stm32f4xx_hal_crc.h"
#endif

#ifdef HAL_CRYP_MODULE_ENABLED
  #include "stm32f4xx_hal_cryp.h"
#endif

#ifdef HAL_DMA2D_MODULE_ENABLED
  #include "stm32f4xx_hal_dma2d.h"
#endif

#ifdef HAL_DAC_MODULE_ENABLED
  #include "stm32f4xx_hal_dac.h"
#endif

#ifdef HAL_DCMI_MODULE_ENABLED
  #include "stm32f4xx_hal_dcmi.h"
#endif

#ifdef HAL_ETH_MODULE_ENABLED
  #include "stm32f4xx_hal_eth.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32f4xx_hal_flash.h"
#endif

#ifdef HAL_SRAM_MODULE_ENABLED
  #include "stm32f4xx_hal_sram.h"
#endif

#ifdef HAL_NOR_MODULE_ENABLED
  #include "stm32f4xx_hal_nor.h"
#endif

#ifdef HAL_NAND_MODULE_ENABLED
  #include "stm32f4xx_hal_nand.h"
#endif

#ifdef HAL_PCCARD_MODULE_ENABLED
  #include "stm32f4xx_hal_pccard.h"
#endif

#ifdef HAL_SDRAM_MODULE_ENABLED
  #include "stm32f4xx_hal_sdram.h"
#endif

#ifdef HAL_HASH_MODULE_ENABLED
  #include "stm32f4xx_hal_hash.h"
#endif

#ifdef HAL_I2C_MODULE_ENABLED
  #include "stm32f4xx_hal_i2c.h"
#endif

#ifdef HAL_I2S_MODULE_ENABLED
  #include "stm32f4xx_hal_i2s.h"
#endif

#ifdef HAL_IWDG_MODULE_ENABLED
  #include "stm32f4xx_hal_iwdg.h"
#endif

#ifdef HAL_LTDC_MODULE_ENABLED
  #include "stm32f4xx_hal_ltdc.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32f4xx_hal_pwr.h"
#endif

#ifdef HAL_RNG_MODULE_ENABLED
  #include "stm32f4xx_hal_rng.h"
#endif

#ifdef HAL_RTC_MODULE_ENABLED
  #include "stm32f4xx_hal_rtc.h"
#endif

#ifdef HAL_SAI_MODULE_ENABLED
  #include "stm32f4xx_hal_sai.h"
#endif

#ifdef HAL_SD_MODULE_ENABLED
  #include "stm32f4xx_hal_sd.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED
  #include "stm32f4xx_hal_spi.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32f4xx_hal_tim.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
  #include "stm32f4xx_hal_uart.h"
#endif

#ifdef HAL_USART_MODULE_ENABLED
  #include "stm32f4xx_hal_usart.h"
#endif

#ifdef HAL_IRDA_MODULE_ENABLED
  #include "stm32f4xx_hal_irda.h"
#endif

#ifdef HAL_SMARTCARD_MODULE_ENABLED
  #include "stm32f4xx_hal_smartcard.h"
#endif

#ifdef HAL_WWDG_MODULE_ENABLED
  #include "stm32f4xx_hal_wwdg.h"
#endif

#ifdef HAL_PCD_MODULE_ENABLED
  #include "stm32f4xx_hal_pcd.h"
#endif

#ifdef HAL_HCD_MODULE_ENABLED
  #include "stm32f4xx_hal_hcd.h"
#endif

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#endif /* STM32F4XX_HAL_CONF_H */
