/**
 * @file tunerstudio.h
 * @brief TunerStudio integration header file for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

#ifndef __TUNERSTUDIO_H
#define __TUNERSTUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  TS_STATE_IDLE = 0,
  TS_STATE_CONNECTED,
  TS_STATE_PROCESSING,
  TS_STATE_ERROR
} TS_State_t;

typedef struct {
  uint8_t enabled;
  uint32_t baudRate;
  uint8_t protocol;
  char signature[16];
  uint8_t pageSize;
  uint8_t pageCount;
} TS_Config_t;

/* Exported constants --------------------------------------------------------*/
#define TS_BUFFER_SIZE            256
#define TS_MAX_PAGES              8
#define TS_MAX_CHANNELS           32

/* Protocol types */
#define TS_PROTOCOL_MS            0
#define TS_PROTOCOL_CUSTOM        1

/* Command codes */
#define TS_CMD_ECHO               0
#define TS_CMD_GET_SIGNATURE      1
#define TS_CMD_GET_VERSION        7
#define TS_CMD_GET_PAGE           0x62
#define TS_CMD_SET_PAGE           0x77
#define TS_CMD_BURN_PAGE          0x42
#define TS_CMD_GET_CHANNELS       0x72

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void TS_Init(void);
void TS_Process(void);
TS_State_t TS_GetState(void);
uint8_t TS_Configure(TS_Config_t* config);
TS_Config_t* TS_GetConfig(void);
uint8_t TS_Start(void);
uint8_t TS_Stop(void);
uint8_t TS_SaveConfig(void);
uint8_t TS_LoadConfig(void);
void TS_ResetConfig(void);
uint8_t TS_GenerateINI(char* buffer, uint16_t bufferSize);

#ifdef __cplusplus
}
#endif

#endif /* __TUNERSTUDIO_H */
