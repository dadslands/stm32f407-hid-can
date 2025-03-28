/**
 * @file web_server.h
 * @brief Web Server header file for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

#ifndef __WEB_SERVER_H
#define __WEB_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  WEB_SERVER_STATE_IDLE = 0,
  WEB_SERVER_STATE_LISTENING,
  WEB_SERVER_STATE_CONNECTED,
  WEB_SERVER_STATE_PROCESSING,
  WEB_SERVER_STATE_SENDING,
  WEB_SERVER_STATE_ERROR
} Web_Server_State_t;

typedef struct {
  uint8_t enabled;
  uint16_t port;
  uint8_t ipAddress[4];
  uint8_t netmask[4];
  uint8_t gateway[4];
  uint8_t dhcpEnabled;
  uint8_t username[16];
  uint8_t password[16];
  uint8_t authEnabled;
} Web_Server_Config_t;

/* Exported constants --------------------------------------------------------*/
#define WEB_SERVER_MAX_CONNECTIONS     2
#define WEB_SERVER_BUFFER_SIZE         2048
#define WEB_SERVER_MAX_URI_LENGTH      128
#define WEB_SERVER_MAX_HEADERS         16
#define WEB_SERVER_MAX_HEADER_LENGTH   128

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void Web_Server_Init(void);
void Web_Server_Process(void);
Web_Server_State_t Web_Server_GetState(void);
uint8_t Web_Server_Configure(Web_Server_Config_t* config);
Web_Server_Config_t* Web_Server_GetConfig(void);
uint8_t Web_Server_Start(void);
uint8_t Web_Server_Stop(void);
uint8_t Web_Server_SaveConfig(void);
uint8_t Web_Server_LoadConfig(void);
void Web_Server_ResetConfig(void);

#ifdef __cplusplus
}
#endif

#endif /* __WEB_SERVER_H */
