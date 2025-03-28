#ifndef STM32F4xx_HAL_H
#define STM32F4xx_HAL_H

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"

/* Only define these if they're not already defined by the STM32CubeF4 library */
#ifndef HAL_StatusTypeDef
/* HAL_StatusTypeDef definition */
typedef enum {
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;
#endif

#ifndef GPIO_PIN_RESET
/* GPIO Definitions */
#define GPIO_PIN_0                 ((uint16_t)0x0001)
#define GPIO_PIN_1                 ((uint16_t)0x0002)
#define GPIO_PIN_2                 ((uint16_t)0x0004)
#define GPIO_PIN_3                 ((uint16_t)0x0008)
#define GPIO_PIN_4                 ((uint16_t)0x0010)
#define GPIO_PIN_5                 ((uint16_t)0x0020)
#define GPIO_PIN_6                 ((uint16_t)0x0040)
#define GPIO_PIN_7                 ((uint16_t)0x0080)
#define GPIO_PIN_8                 ((uint16_t)0x0100)
#define GPIO_PIN_9                 ((uint16_t)0x0200)
#define GPIO_PIN_10                ((uint16_t)0x0400)
#define GPIO_PIN_11                ((uint16_t)0x0800)
#define GPIO_PIN_12                ((uint16_t)0x1000)
#define GPIO_PIN_13                ((uint16_t)0x2000)
#define GPIO_PIN_14                ((uint16_t)0x4000)
#define GPIO_PIN_15                ((uint16_t)0x8000)
#define GPIO_PIN_All               ((uint16_t)0xFFFF)

#define GPIO_PIN_RESET             0U
#define GPIO_PIN_SET               1U
#endif

#ifndef GPIO_AF5_SPI1
/* GPIO Alternate Function definitions */
#define GPIO_AF0_RTC_50Hz      ((uint8_t)0x00)
#define GPIO_AF1_TIM1          ((uint8_t)0x01)
#define GPIO_AF2_TIM3          ((uint8_t)0x02)
#define GPIO_AF3_TIM9          ((uint8_t)0x03)
#define GPIO_AF4_I2C1          ((uint8_t)0x04)
#define GPIO_AF5_SPI1          ((uint8_t)0x05)
#define GPIO_AF6_SPI3          ((uint8_t)0x06)
#define GPIO_AF7_USART1        ((uint8_t)0x07)
#define GPIO_AF7_USART2        ((uint8_t)0x07)
#define GPIO_AF8_USART6        ((uint8_t)0x08)
#define GPIO_AF9_TIM14         ((uint8_t)0x09)
#define GPIO_AF9_CAN1          ((uint8_t)0x09)
#define GPIO_AF10_OTG_FS       ((uint8_t)0x0A)
#define GPIO_AF11_ETH          ((uint8_t)0x0B)
#define GPIO_AF12_OTG_HS       ((uint8_t)0x0C)
#define GPIO_AF13_DCMI         ((uint8_t)0x0D)
#define GPIO_AF15_EVENTOUT     ((uint8_t)0x0F)
#endif

/* External variables */
extern SPI_HandleTypeDef hspi1;

#endif /* STM32F4xx_HAL_H */
