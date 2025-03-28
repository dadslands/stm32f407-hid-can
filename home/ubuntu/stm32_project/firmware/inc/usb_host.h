/**
 * @file usb_host.h
 * @brief USB Host interface for HID devices
 * @author Manus AI
 * @date March 2025
 * 
 * This file contains the declarations for the USB Host functionality
 * that handles HID device connections, enumeration, and data reception.
 * It provides an interface for the Input Manager to receive data from
 * connected HID devices like keyboards, mice, and gamepads.
 * 
 * References:
 * - STM32 USB Host Library: https://www.st.com/resource/en/user_manual/dm00108129.pdf
 * - USB HID Specification: https://www.usb.org/document-library/device-class-definition-hid-111
 * - USB 2.0 Specification: https://www.usb.org/document-library/usb-20-specification
 */

#ifndef __USB_HOST_H
#define __USB_HOST_H

#include "main.h"
#include <stdint.h>

/* Maximum number of supported HID devices */
#define MAX_HID_DEVICES 4

/* HID device types */
typedef enum {
  HID_DEVICE_UNKNOWN = 0,
  HID_DEVICE_KEYBOARD,
  HID_DEVICE_MOUSE,
  HID_DEVICE_GAMEPAD,
  HID_DEVICE_JOYSTICK
} HID_DeviceType_t;

/* HID device state */
typedef enum {
  HID_DEVICE_DISCONNECTED = 0,
  HID_DEVICE_CONNECTED,
  HID_DEVICE_READY,
  HID_DEVICE_ERROR
} HID_DeviceState_t;

/* HID device information structure */
typedef struct {
  uint8_t id;                  /* Device ID (index) */
  HID_DeviceType_t type;       /* Device type (keyboard, mouse, etc.) */
  HID_DeviceState_t state;     /* Current device state */
  uint16_t vendorId;           /* USB vendor ID */
  uint16_t productId;          /* USB product ID */
  char manufacturer[64];       /* Manufacturer string */
  char product[64];            /* Product string */
  char serialNumber[64];       /* Serial number string */
  uint8_t interface;           /* Interface number */
  uint8_t inEp;                /* Input endpoint address */
  uint8_t outEp;               /* Output endpoint address (if available) */
  uint16_t pollInterval;       /* Polling interval in ms */
  uint8_t reportDescriptor[256]; /* HID report descriptor */
  uint16_t reportDescriptorLength; /* Length of report descriptor */
} HID_DeviceInfo_t;

/**
 * @brief Initialize the USB Host for HID devices
 * 
 * This function initializes the USB Host stack and prepares it
 * for HID device connections. It sets up the necessary hardware
 * and software components for USB communication.
 * 
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef MX_USB_HOST_Init(void);

/**
 * @brief Process USB Host events
 * 
 * This function must be called regularly in the main loop to
 * process USB Host events, including device connection/disconnection
 * and data reception.
 * 
 * @return None
 */
void MX_USB_HOST_Process(void);

/**
 * @brief Get information about a connected HID device
 * 
 * This function retrieves information about a connected HID device
 * based on its ID.
 * 
 * @param deviceId Device ID to query
 * @param deviceInfo Pointer to store device information
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef USB_HOST_GetDeviceInfo(uint8_t deviceId, HID_DeviceInfo_t *deviceInfo);

/**
 * @brief Get the number of connected HID devices
 * 
 * This function returns the number of currently connected HID devices.
 * 
 * @return uint8_t Number of connected devices
 */
uint8_t USB_HOST_GetConnectedDeviceCount(void);

/**
 * @brief Send data to an HID device
 * 
 * This function sends data to a connected HID device, if the device
 * supports output reports.
 * 
 * @param deviceId Device ID to send data to
 * @param data Pointer to data buffer
 * @param length Length of data to send
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef USB_HOST_SendData(uint8_t deviceId, uint8_t *data, uint16_t length);

/**
 * @brief Register a callback for HID device connection events
 * 
 * This function registers a callback that will be called when
 * an HID device is connected or disconnected.
 * 
 * @param callback Function pointer to the callback
 * @return None
 */
void USB_HOST_RegisterConnectionCallback(void (*callback)(uint8_t deviceId, HID_DeviceState_t state));

/**
 * @brief Register a callback for HID device data reception
 * 
 * This function registers a callback that will be called when
 * data is received from an HID device.
 * 
 * @param callback Function pointer to the callback
 * @return None
 */
void USB_HOST_RegisterDataCallback(void (*callback)(uint8_t deviceId, uint8_t *data, uint16_t length));

/**
 * @brief Reset the USB Host
 * 
 * This function resets the USB Host stack and re-initializes it.
 * It can be used to recover from error states or to force
 * re-enumeration of connected devices.
 * 
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef USB_HOST_Reset(void);

/**
 * @brief Get the last USB Host error
 * 
 * This function returns the last error that occurred in the USB Host.
 * 
 * @return uint32_t Error code
 */
uint32_t USB_HOST_GetLastError(void);

/**
 * @brief Debug function to print USB Host status
 * 
 * This function prints detailed information about the USB Host
 * status, including connected devices and their states.
 * 
 * @return None
 */
void USB_HOST_DebugPrint(void);

#endif /* __USB_HOST_H */
