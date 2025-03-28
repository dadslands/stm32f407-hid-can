/**
 * @file usb_host.c
 * @brief USB Host implementation for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

/* Includes ------------------------------------------------------------------*/
#include "usb_host.h"
#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBH_HandleTypeDef hUsbHostFS;
HID_Device_Info_t hidDevices[MAX_HID_DEVICES];
uint8_t deviceCount = 0;
USB_Host_State_t hostState = USB_HOST_IDLE;
void (*userCallback)(HID_Device_Info_t* deviceInfo) = NULL;

/* Private function prototypes -----------------------------------------------*/
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
static HID_Device_Type_t USB_Host_DetermineDeviceType(USBH_HandleTypeDef *phost);
static void USB_Host_ParseHIDReport(USBH_HandleTypeDef *phost, uint8_t deviceIndex);

/* External variables --------------------------------------------------------*/

/**
  * @brief  USB Host initialization function
  * @param  None
  * @retval None
  */
void USB_Host_Init(void)
{
  /* Init Host Library */
  USBH_Init(&hUsbHostFS, USBH_UserProcess, 0);
  
  /* Add Supported Class */
  USBH_RegisterClass(&hUsbHostFS, USBH_HID_CLASS);
  
  /* Start Host Process */
  USBH_Start(&hUsbHostFS);
  
  /* Initialize device info array */
  for (uint8_t i = 0; i < MAX_HID_DEVICES; i++) {
    hidDevices[i].isConnected = 0;
    hidDevices[i].deviceIndex = i;
    hidDevices[i].deviceType = HID_DEVICE_UNKNOWN;
    hidDevices[i].reportDataLength = 0;
  }
  
  deviceCount = 0;
  hostState = USB_HOST_IDLE;
}

/**
  * @brief  USB Host process function, should be called periodically
  * @param  None
  * @retval None
  */
void USB_Host_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostFS);
  
  /* Process connected devices */
  for (uint8_t i = 0; i < deviceCount; i++) {
    if (hidDevices[i].isConnected) {
      USB_Host_ParseHIDReport(&hUsbHostFS, i);
    }
  }
}

/**
  * @brief  Get current USB Host state
  * @param  None
  * @retval USB_Host_State_t: Current state
  */
USB_Host_State_t USB_Host_GetState(void)
{
  return hostState;
}

/**
  * @brief  Get number of connected HID devices
  * @param  None
  * @retval uint8_t: Number of connected devices
  */
uint8_t USB_Host_GetDeviceCount(void)
{
  return deviceCount;
}

/**
  * @brief  Get information about a specific HID device
  * @param  deviceIndex: Index of the device
  * @retval HID_Device_Info_t*: Pointer to device information structure
  */
HID_Device_Info_t* USB_Host_GetDeviceInfo(uint8_t deviceIndex)
{
  if (deviceIndex < deviceCount) {
    return &hidDevices[deviceIndex];
  }
  return NULL;
}

/**
  * @brief  Get the latest report from a specific HID device
  * @param  deviceIndex: Index of the device
  * @param  buffer: Buffer to store the report
  * @param  bufferSize: Size of the buffer
  * @retval uint8_t: Length of the report data copied to buffer
  */
uint8_t USB_Host_GetDeviceReport(uint8_t deviceIndex, uint8_t* buffer, uint8_t bufferSize)
{
  if (deviceIndex < deviceCount && hidDevices[deviceIndex].isConnected) {
    uint8_t copyLength = (bufferSize < hidDevices[deviceIndex].reportDataLength) ? 
                          bufferSize : hidDevices[deviceIndex].reportDataLength;
    
    memcpy(buffer, hidDevices[deviceIndex].lastReportData, copyLength);
    return copyLength;
  }
  return 0;
}

/**
  * @brief  Register a callback function for HID device events
  * @param  callback: Pointer to callback function
  * @retval None
  */
void USB_Host_RegisterCallback(void (*callback)(HID_Device_Info_t* deviceInfo))
{
  userCallback = callback;
}

/**
  * @brief  User Process function for USB Host events
  * @param  phost: USB Host handle
  * @param  id: Event ID
  * @retval None
  */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  switch (id) {
    case HOST_USER_SELECT_CONFIGURATION:
      break;
    
    case HOST_USER_DISCONNECTION:
      hostState = USB_HOST_DEVICE_DISCONNECTED;
      
      /* Mark all devices as disconnected for now - in a more sophisticated implementation,
         we would track which specific device was disconnected */
      for (uint8_t i = 0; i < deviceCount; i++) {
        hidDevices[i].isConnected = 0;
        
        /* Call user callback if registered */
        if (userCallback != NULL) {
          userCallback(&hidDevices[i]);
        }
      }
      break;
    
    case HOST_USER_CLASS_ACTIVE:
      hostState = USB_HOST_DEVICE_CLASS_ACTIVE;
      
      /* New device connected */
      if (deviceCount < MAX_HID_DEVICES) {
        /* Get device information */
        hidDevices[deviceCount].vendorId = phost->device.DevDesc.idVendor;
        hidDevices[deviceCount].productId = phost->device.DevDesc.idProduct;
        hidDevices[deviceCount].deviceType = USB_Host_DetermineDeviceType(phost);
        hidDevices[deviceCount].isConnected = 1;
        hidDevices[deviceCount].reportDataLength = 0;
        
        /* Generate device name based on type and index */
        switch (hidDevices[deviceCount].deviceType) {
          case HID_DEVICE_KEYBOARD:
            sprintf(hidDevices[deviceCount].deviceName, "Keyboard %d", deviceCount);
            break;
          case HID_DEVICE_MOUSE:
            sprintf(hidDevices[deviceCount].deviceName, "Mouse %d", deviceCount);
            break;
          case HID_DEVICE_GAMEPAD:
            sprintf(hidDevices[deviceCount].deviceName, "Gamepad %d", deviceCount);
            break;
          case HID_DEVICE_JOYSTICK:
            sprintf(hidDevices[deviceCount].deviceName, "Joystick %d", deviceCount);
            break;
          default:
            sprintf(hidDevices[deviceCount].deviceName, "HID Device %d", deviceCount);
            break;
        }
        
        /* Call user callback if registered */
        if (userCallback != NULL) {
          userCallback(&hidDevices[deviceCount]);
        }
        
        deviceCount++;
      }
      break;
    
    case HOST_USER_CONNECTION:
      hostState = USB_HOST_DEVICE_CONNECTED;
      break;
      
    default:
      break;
  }
}

/**
  * @brief  Determine the type of HID device based on descriptors
  * @param  phost: USB Host handle
  * @retval HID_Device_Type_t: Determined device type
  */
static HID_Device_Type_t USB_Host_DetermineDeviceType(USBH_HandleTypeDef *phost)
{
  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
  
  /* Check for device type based on usage page and usage values */
  if (HID_Handle->HID_Desc.RptDesc[0] == 0x05 && HID_Handle->HID_Desc.RptDesc[1] == 0x01) {
    /* Generic Desktop Controls usage page */
    
    /* Check usage */
    if (HID_Handle->HID_Desc.RptDesc[2] == 0x09) {
      switch (HID_Handle->HID_Desc.RptDesc[3]) {
        case 0x06:
          return HID_DEVICE_KEYBOARD;
        case 0x02:
          return HID_DEVICE_MOUSE;
        case 0x04:
          return HID_DEVICE_JOYSTICK;
        case 0x05:
          return HID_DEVICE_GAMEPAD;
        default:
          break;
      }
    }
  }
  
  /* If we can't determine the type, check common vendor/product IDs */
  uint16_t vendorId = phost->device.DevDesc.idVendor;
  uint16_t productId = phost->device.DevDesc.idProduct;
  
  /* This is where you would add known device IDs for specific types */
  
  /* Default to unknown type */
  return HID_DEVICE_CUSTOM;
}

/**
  * @brief  Parse HID report data from a device
  * @param  phost: USB Host handle
  * @param  deviceIndex: Index of the device
  * @retval None
  */
static void USB_Host_ParseHIDReport(USBH_HandleTypeDef *phost, uint8_t deviceIndex)
{
  if (deviceIndex >= deviceCount || !hidDevices[deviceIndex].isConnected) {
    return;
  }
  
  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
  
  /* Get latest report data */
  if (USBH_HID_GetReport(phost, 0, 0, hidDevices[deviceIndex].lastReportData, 
                         HID_REPORT_BUFFER_SIZE) == USBH_OK) {
    
    /* Update report data length based on device type */
    switch (hidDevices[deviceIndex].deviceType) {
      case HID_DEVICE_KEYBOARD:
        hidDevices[deviceIndex].reportDataLength = 8; /* Standard keyboard report size */
        break;
      case HID_DEVICE_MOUSE:
        hidDevices[deviceIndex].reportDataLength = 4; /* Standard mouse report size */
        break;
      case HID_DEVICE_GAMEPAD:
      case HID_DEVICE_JOYSTICK:
        hidDevices[deviceIndex].reportDataLength = HID_Handle->length;
        break;
      default:
        hidDevices[deviceIndex].reportDataLength = HID_Handle->length;
        break;
    }
    
    /* Call user callback if registered */
    if (userCallback != NULL) {
      userCallback(&hidDevices[deviceIndex]);
    }
  }
}
