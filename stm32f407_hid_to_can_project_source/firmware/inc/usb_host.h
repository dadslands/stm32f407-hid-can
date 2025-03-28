/**
 * @file usb_host.h
 * @brief USB Host header file for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

#ifndef __USB_HOST_H
#define __USB_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "usbh_core.h"
#include "usbh_hid.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  USB_HOST_IDLE = 0,
  USB_HOST_DEVICE_CONNECTED,
  USB_HOST_DEVICE_DISCONNECTED,
  USB_HOST_DEVICE_ENUMERATION_COMPLETE,
  USB_HOST_DEVICE_CLASS_ACTIVE,
  USB_HOST_ERROR
} USB_Host_State_t;

typedef enum {
  HID_DEVICE_UNKNOWN = 0,
  HID_DEVICE_KEYBOARD,
  HID_DEVICE_MOUSE,
  HID_DEVICE_GAMEPAD,
  HID_DEVICE_JOYSTICK,
  HID_DEVICE_CUSTOM
} HID_Device_Type_t;

typedef struct {
  uint16_t vendorId;
  uint16_t productId;
  HID_Device_Type_t deviceType;
  uint8_t deviceIndex;
  char deviceName[32];
  uint8_t isConnected;
  uint8_t lastReportData[64];
  uint8_t reportDataLength;
} HID_Device_Info_t;

/* Exported constants --------------------------------------------------------*/
#define MAX_HID_DEVICES            8
#define HID_REPORT_BUFFER_SIZE     64

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void USB_Host_Init(void);
void USB_Host_Process(void);
USB_Host_State_t USB_Host_GetState(void);
uint8_t USB_Host_GetDeviceCount(void);
HID_Device_Info_t* USB_Host_GetDeviceInfo(uint8_t deviceIndex);
uint8_t USB_Host_GetDeviceReport(uint8_t deviceIndex, uint8_t* buffer, uint8_t bufferSize);
void USB_Host_RegisterCallback(void (*callback)(HID_Device_Info_t* deviceInfo));

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_H */
