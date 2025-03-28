/**
 * @file main.h
 * @brief Main header file for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* GPIO Definitions for STM32mega board */
/* LED */
#define LED_PIN                     GPIO_PIN_13
#define LED_GPIO_PORT               GPIOC
#define LED_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()

/* USB Host */
#define USB_HOST_FS_ID              0
#define USB_HOST_HS_ID              1

/* Display SPI */
#define DISPLAY_SPI                 SPI1
#define DISPLAY_SPI_CLK_ENABLE()    __HAL_RCC_SPI1_CLK_ENABLE()

/* Display GPIO */
#define DISPLAY_CS_PIN              GPIO_PIN_4
#define DISPLAY_CS_PORT             GPIOA
#define DISPLAY_CS_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define DISPLAY_DC_PIN              GPIO_PIN_3
#define DISPLAY_DC_PORT             GPIOA
#define DISPLAY_DC_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define DISPLAY_RST_PIN             GPIO_PIN_2
#define DISPLAY_RST_PORT            GPIOA
#define DISPLAY_RST_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()

#define DISPLAY_BL_PIN              GPIO_PIN_1
#define DISPLAY_BL_PORT             GPIOA
#define DISPLAY_BL_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define DISPLAY_SCK_PIN             GPIO_PIN_5
#define DISPLAY_SCK_PORT            GPIOA
#define DISPLAY_SCK_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define DISPLAY_SCK_AF              GPIO_AF5_SPI1

#define DISPLAY_MISO_PIN            GPIO_PIN_6
#define DISPLAY_MISO_PORT           GPIOA
#define DISPLAY_MISO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define DISPLAY_MISO_AF             GPIO_AF5_SPI1

#define DISPLAY_MOSI_PIN            GPIO_PIN_7
#define DISPLAY_MOSI_PORT           GPIOA
#define DISPLAY_MOSI_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define DISPLAY_MOSI_AF             GPIO_AF5_SPI1

/* UART */
#define UART_TX_PIN                 GPIO_PIN_2
#define UART_TX_PORT                GPIOA
#define UART_TX_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART_TX_AF                  GPIO_AF7_USART2

#define UART_RX_PIN                 GPIO_PIN_3
#define UART_RX_PORT                GPIOA
#define UART_RX_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART_RX_AF                  GPIO_AF7_USART2

/* CAN */
#define CAN_TX_PIN                  GPIO_PIN_12
#define CAN_TX_PORT                 GPIOA
#define CAN_TX_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define CAN_TX_AF                   GPIO_AF9_CAN1

#define CAN_RX_PIN                  GPIO_PIN_11
#define CAN_RX_PORT                 GPIOA
#define CAN_RX_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define CAN_RX_AF                   GPIO_AF9_CAN1

/* Ethernet */
#define ETH_MDC_PIN                 GPIO_PIN_1
#define ETH_MDC_PORT                GPIOC
#define ETH_MDC_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define ETH_MDC_AF                  GPIO_AF11_ETH

#define ETH_MDIO_PIN                GPIO_PIN_2
#define ETH_MDIO_PORT               GPIOA
#define ETH_MDIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define ETH_MDIO_AF                 GPIO_AF11_ETH

#define ETH_REF_CLK_PIN             GPIO_PIN_1
#define ETH_REF_CLK_PORT            GPIOA
#define ETH_REF_CLK_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define ETH_REF_CLK_AF              GPIO_AF11_ETH

#define ETH_CRS_DV_PIN              GPIO_PIN_7
#define ETH_CRS_DV_PORT             GPIOA
#define ETH_CRS_DV_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define ETH_CRS_DV_AF               GPIO_AF11_ETH

#define ETH_TX_EN_PIN               GPIO_PIN_11
#define ETH_TX_EN_PORT              GPIOB
#define ETH_TX_EN_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define ETH_TX_EN_AF                GPIO_AF11_ETH

#define ETH_TXD0_PIN                GPIO_PIN_12
#define ETH_TXD0_PORT               GPIOB
#define ETH_TXD0_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define ETH_TXD0_AF                 GPIO_AF11_ETH

#define ETH_TXD1_PIN                GPIO_PIN_13
#define ETH_TXD1_PORT               GPIOB
#define ETH_TXD1_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define ETH_TXD1_AF                 GPIO_AF11_ETH

#define ETH_RXD0_PIN                GPIO_PIN_4
#define ETH_RXD0_PORT               GPIOC
#define ETH_RXD0_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define ETH_RXD0_AF                 GPIO_AF11_ETH

#define ETH_RXD1_PIN                GPIO_PIN_5
#define ETH_RXD1_PORT               GPIOC
#define ETH_RXD1_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define ETH_RXD1_AF                 GPIO_AF11_ETH

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
