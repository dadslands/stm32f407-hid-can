/**
 * @file input_manager.h
 * @brief Input Manager header file for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

#ifndef __INPUT_MANAGER_H
#define __INPUT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "usb_host.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  INPUT_EVENT_NONE = 0,
  INPUT_EVENT_BUTTON_PRESS,
  INPUT_EVENT_BUTTON_RELEASE,
  INPUT_EVENT_AXIS_CHANGE,
  INPUT_EVENT_KEY_PRESS,
  INPUT_EVENT_KEY_RELEASE
} Input_Event_Type_t;

typedef struct {
  Input_Event_Type_t eventType;
  uint8_t deviceIndex;
  uint8_t inputId;
  int16_t value;
  uint32_t timestamp;
} Input_Event_t;

/* Exported constants --------------------------------------------------------*/
#define MAX_INPUT_QUEUE_SIZE       32
#define MAX_INPUT_MAPPINGS         64

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void Input_Manager_Init(void);
void Input_Manager_Process(void);
uint8_t Input_Manager_GetEventCount(void);
Input_Event_t* Input_Manager_GetNextEvent(void);
void Input_Manager_RegisterCallback(void (*callback)(Input_Event_t* inputEvent));
uint8_t Input_Manager_GetDeviceCount(void);
HID_Device_Info_t* Input_Manager_GetDeviceInfo(uint8_t deviceIndex);

#ifdef __cplusplus
}
#endif

#endif /* __INPUT_MANAGER_H */
