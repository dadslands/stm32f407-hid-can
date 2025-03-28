/**
 * @file output_manager.h
 * @brief Output Manager header file for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

#ifndef __OUTPUT_MANAGER_H
#define __OUTPUT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  SERIAL_FORMAT_RAW = 0,
  SERIAL_FORMAT_ASCII,
  SERIAL_FORMAT_BINARY_LSB,
  SERIAL_FORMAT_BINARY_MSB,
  SERIAL_FORMAT_HEX
} Serial_Format_t;

typedef struct {
  uint8_t enabled;
  uint32_t baudRate;
  uint8_t dataBits;
  uint8_t stopBits;
  uint8_t parity;
  Serial_Format_t format;
} Serial_Config_t;

typedef struct {
  uint8_t enabled;
  uint32_t bitRate;
  uint8_t mode;
  uint8_t sjw;
  uint8_t bs1;
  uint8_t bs2;
} CAN_Config_t;

/* Exported constants --------------------------------------------------------*/
#define MAX_SERIAL_BUFFER_SIZE    256
#define MAX_CAN_BUFFER_SIZE       64

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void Output_Manager_Init(void);
void Output_Manager_Process(void);
uint8_t Output_Manager_SendSerial(uint8_t* data, uint8_t length);
uint8_t Output_Manager_SendCAN(uint32_t canId, uint8_t* data, uint8_t length);
uint8_t Output_Manager_ConfigureSerial(Serial_Config_t* config);
uint8_t Output_Manager_ConfigureCAN(CAN_Config_t* config);
Serial_Config_t* Output_Manager_GetSerialConfig(void);
CAN_Config_t* Output_Manager_GetCANConfig(void);
uint8_t Output_Manager_SaveConfig(void);
uint8_t Output_Manager_LoadConfig(void);
void Output_Manager_ResetConfig(void);

#ifdef __cplusplus
}
#endif

#endif /* __OUTPUT_MANAGER_H */
