/**
 * @file input_manager.c
 * @brief Input Manager implementation for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

/* Includes ------------------------------------------------------------------*/
#include "input_manager.h"
#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Input_Event_t inputEventQueue[MAX_INPUT_QUEUE_SIZE];
uint8_t queueHead = 0;
uint8_t queueTail = 0;
uint8_t eventCount = 0;
void (*userCallback)(Input_Event_t* inputEvent) = NULL;

/* Private function prototypes -----------------------------------------------*/
static void Input_Manager_HID_Callback(HID_Device_Info_t* deviceInfo);
static void Input_Manager_ProcessKeyboard(HID_Device_Info_t* deviceInfo);
static void Input_Manager_ProcessMouse(HID_Device_Info_t* deviceInfo);
static void Input_Manager_ProcessGamepad(HID_Device_Info_t* deviceInfo);
static void Input_Manager_ProcessJoystick(HID_Device_Info_t* deviceInfo);
static void Input_Manager_ProcessCustomHID(HID_Device_Info_t* deviceInfo);
static void Input_Manager_AddEvent(Input_Event_Type_t eventType, uint8_t deviceIndex, uint8_t inputId, int16_t value);

/* External variables --------------------------------------------------------*/

/**
  * @brief  Input Manager initialization function
  * @param  None
  * @retval None
  */
void Input_Manager_Init(void)
{
  /* Initialize USB Host */
  USB_Host_Init();
  
  /* Register callback for HID device events */
  USB_Host_RegisterCallback(Input_Manager_HID_Callback);
  
  /* Initialize event queue */
  queueHead = 0;
  queueTail = 0;
  eventCount = 0;
}

/**
  * @brief  Input Manager process function, should be called periodically
  * @param  None
  * @retval None
  */
void Input_Manager_Process(void)
{
  /* Process USB Host */
  USB_Host_Process();
}

/**
  * @brief  Get number of events in the queue
  * @param  None
  * @retval uint8_t: Number of events
  */
uint8_t Input_Manager_GetEventCount(void)
{
  return eventCount;
}

/**
  * @brief  Get next event from the queue
  * @param  None
  * @retval Input_Event_t*: Pointer to the next event, NULL if queue is empty
  */
Input_Event_t* Input_Manager_GetNextEvent(void)
{
  if (eventCount == 0) {
    return NULL;
  }
  
  Input_Event_t* event = &inputEventQueue[queueHead];
  
  /* Update queue head */
  queueHead = (queueHead + 1) % MAX_INPUT_QUEUE_SIZE;
  eventCount--;
  
  return event;
}

/**
  * @brief  Register a callback function for input events
  * @param  callback: Pointer to callback function
  * @retval None
  */
void Input_Manager_RegisterCallback(void (*callback)(Input_Event_t* inputEvent))
{
  userCallback = callback;
}

/**
  * @brief  Get number of connected HID devices
  * @param  None
  * @retval uint8_t: Number of connected devices
  */
uint8_t Input_Manager_GetDeviceCount(void)
{
  return USB_Host_GetDeviceCount();
}

/**
  * @brief  Get information about a specific HID device
  * @param  deviceIndex: Index of the device
  * @retval HID_Device_Info_t*: Pointer to device information structure
  */
HID_Device_Info_t* Input_Manager_GetDeviceInfo(uint8_t deviceIndex)
{
  return USB_Host_GetDeviceInfo(deviceIndex);
}

/**
  * @brief  Callback function for HID device events
  * @param  deviceInfo: Pointer to device information structure
  * @retval None
  */
static void Input_Manager_HID_Callback(HID_Device_Info_t* deviceInfo)
{
  if (deviceInfo == NULL) {
    return;
  }
  
  /* Process device based on its type */
  switch (deviceInfo->deviceType) {
    case HID_DEVICE_KEYBOARD:
      Input_Manager_ProcessKeyboard(deviceInfo);
      break;
    
    case HID_DEVICE_MOUSE:
      Input_Manager_ProcessMouse(deviceInfo);
      break;
    
    case HID_DEVICE_GAMEPAD:
      Input_Manager_ProcessGamepad(deviceInfo);
      break;
    
    case HID_DEVICE_JOYSTICK:
      Input_Manager_ProcessJoystick(deviceInfo);
      break;
    
    case HID_DEVICE_CUSTOM:
    case HID_DEVICE_UNKNOWN:
    default:
      Input_Manager_ProcessCustomHID(deviceInfo);
      break;
  }
}

/**
  * @brief  Process keyboard HID report
  * @param  deviceInfo: Pointer to device information structure
  * @retval None
  */
static void Input_Manager_ProcessKeyboard(HID_Device_Info_t* deviceInfo)
{
  static uint8_t prevKeys[6] = {0};
  
  /* Standard keyboard report format:
     Byte 0: Modifier keys (CTRL, SHIFT, ALT, etc.)
     Byte 1: Reserved
     Bytes 2-7: Key codes (up to 6 keys pressed simultaneously) */
  
  uint8_t modifiers = deviceInfo->lastReportData[0];
  
  /* Check for modifier key changes */
  static uint8_t prevModifiers = 0;
  uint8_t changedModifiers = modifiers ^ prevModifiers;
  
  /* Process modifier key changes */
  for (uint8_t i = 0; i < 8; i++) {
    if (changedModifiers & (1 << i)) {
      if (modifiers & (1 << i)) {
        /* Modifier key pressed */
        Input_Manager_AddEvent(INPUT_EVENT_KEY_PRESS, deviceInfo->deviceIndex, i, 1);
      } else {
        /* Modifier key released */
        Input_Manager_AddEvent(INPUT_EVENT_KEY_RELEASE, deviceInfo->deviceIndex, i, 0);
      }
    }
  }
  prevModifiers = modifiers;
  
  /* Process regular keys */
  /* First, check for released keys */
  for (uint8_t i = 0; i < 6; i++) {
    if (prevKeys[i] != 0) {
      uint8_t stillPressed = 0;
      
      /* Check if key is still in the current report */
      for (uint8_t j = 0; j < 6; j++) {
        if (prevKeys[i] == deviceInfo->lastReportData[j + 2]) {
          stillPressed = 1;
          break;
        }
      }
      
      if (!stillPressed) {
        /* Key was released */
        Input_Manager_AddEvent(INPUT_EVENT_KEY_RELEASE, deviceInfo->deviceIndex, prevKeys[i], 0);
        prevKeys[i] = 0;
      }
    }
  }
  
  /* Then, check for newly pressed keys */
  for (uint8_t i = 0; i < 6; i++) {
    uint8_t keyCode = deviceInfo->lastReportData[i + 2];
    
    if (keyCode != 0) {
      uint8_t isNewKey = 1;
      
      /* Check if key was already in the previous report */
      for (uint8_t j = 0; j < 6; j++) {
        if (keyCode == prevKeys[j]) {
          isNewKey = 0;
          break;
        }
      }
      
      if (isNewKey) {
        /* New key press */
        Input_Manager_AddEvent(INPUT_EVENT_KEY_PRESS, deviceInfo->deviceIndex, keyCode, 1);
        
        /* Store key in previous keys array */
        for (uint8_t j = 0; j < 6; j++) {
          if (prevKeys[j] == 0) {
            prevKeys[j] = keyCode;
            break;
          }
        }
      }
    }
  }
}

/**
  * @brief  Process mouse HID report
  * @param  deviceInfo: Pointer to device information structure
  * @retval None
  */
static void Input_Manager_ProcessMouse(HID_Device_Info_t* deviceInfo)
{
  /* Standard mouse report format:
     Byte 0: Button states
     Byte 1: X movement
     Byte 2: Y movement
     Byte 3: Wheel movement (if available) */
  
  uint8_t buttons = deviceInfo->lastReportData[0];
  int8_t x = (int8_t)deviceInfo->lastReportData[1];
  int8_t y = (int8_t)deviceInfo->lastReportData[2];
  int8_t wheel = 0;
  
  if (deviceInfo->reportDataLength > 3) {
    wheel = (int8_t)deviceInfo->lastReportData[3];
  }
  
  /* Check for button changes */
  static uint8_t prevButtons = 0;
  uint8_t changedButtons = buttons ^ prevButtons;
  
  /* Process button changes */
  for (uint8_t i = 0; i < 3; i++) {  /* Most mice have 3 buttons */
    if (changedButtons & (1 << i)) {
      if (buttons & (1 << i)) {
        /* Button pressed */
        Input_Manager_AddEvent(INPUT_EVENT_BUTTON_PRESS, deviceInfo->deviceIndex, i, 1);
      } else {
        /* Button released */
        Input_Manager_AddEvent(INPUT_EVENT_BUTTON_RELEASE, deviceInfo->deviceIndex, i, 0);
      }
    }
  }
  prevButtons = buttons;
  
  /* Process movement */
  if (x != 0) {
    Input_Manager_AddEvent(INPUT_EVENT_AXIS_CHANGE, deviceInfo->deviceIndex, 0, x);
  }
  
  if (y != 0) {
    Input_Manager_AddEvent(INPUT_EVENT_AXIS_CHANGE, deviceInfo->deviceIndex, 1, y);
  }
  
  if (wheel != 0) {
    Input_Manager_AddEvent(INPUT_EVENT_AXIS_CHANGE, deviceInfo->deviceIndex, 2, wheel);
  }
}

/**
  * @brief  Process gamepad HID report
  * @param  deviceInfo: Pointer to device information structure
  * @retval None
  */
static void Input_Manager_ProcessGamepad(HID_Device_Info_t* deviceInfo)
{
  /* Gamepad reports vary by manufacturer, but typically include:
     - Button states (usually as a bit field)
     - Analog stick positions
     - Trigger values
     
     This is a simplified implementation that assumes a common format.
     For a production system, you would need to parse the HID descriptor
     to understand the report format for each specific gamepad. */
  
  /* Assume first byte contains button states */
  uint8_t buttons = deviceInfo->lastReportData[0];
  
  /* Check for button changes */
  static uint8_t prevButtons = 0;
  uint8_t changedButtons = buttons ^ prevButtons;
  
  /* Process button changes */
  for (uint8_t i = 0; i < 8; i++) {
    if (changedButtons & (1 << i)) {
      if (buttons & (1 << i)) {
        /* Button pressed */
        Input_Manager_AddEvent(INPUT_EVENT_BUTTON_PRESS, deviceInfo->deviceIndex, i, 1);
      } else {
        /* Button released */
        Input_Manager_AddEvent(INPUT_EVENT_BUTTON_RELEASE, deviceInfo->deviceIndex, i, 0);
      }
    }
  }
  prevButtons = buttons;
  
  /* Assume subsequent bytes contain analog values */
  for (uint8_t i = 1; i < deviceInfo->reportDataLength; i++) {
    /* Compare with previous value */
    static uint8_t prevAnalog[16] = {0};
    
    if (i < 17 && deviceInfo->lastReportData[i] != prevAnalog[i-1]) {
      /* Analog value changed */
      Input_Manager_AddEvent(INPUT_EVENT_AXIS_CHANGE, deviceInfo->deviceIndex, i-1, deviceInfo->lastReportData[i]);
      prevAnalog[i-1] = deviceInfo->lastReportData[i];
    }
  }
}

/**
  * @brief  Process joystick HID report
  * @param  deviceInfo: Pointer to device information structure
  * @retval None
  */
static void Input_Manager_ProcessJoystick(HID_Device_Info_t* deviceInfo)
{
  /* Joystick processing is similar to gamepad */
  Input_Manager_ProcessGamepad(deviceInfo);
}

/**
  * @brief  Process custom HID device report
  * @param  deviceInfo: Pointer to device information structure
  * @retval None
  */
static void Input_Manager_ProcessCustomHID(HID_Device_Info_t* deviceInfo)
{
  /* For custom HID devices, we would need to know the report format
     to properly interpret the data. For now, we'll just generate
     generic events for any changes in the report data. */
  
  static uint8_t prevData[HID_REPORT_BUFFER_SIZE] = {0};
  
  for (uint8_t i = 0; i < deviceInfo->reportDataLength; i++) {
    if (deviceInfo->lastReportData[i] != prevData[i]) {
      /* Data changed */
      Input_Manager_AddEvent(INPUT_EVENT_AXIS_CHANGE, deviceInfo->deviceIndex, i, deviceInfo->lastReportData[i]);
      prevData[i] = deviceInfo->lastReportData[i];
    }
  }
}

/**
  * @brief  Add an event to the queue
  * @param  eventType: Type of the event
  * @param  deviceIndex: Index of the device
  * @param  inputId: ID of the input (button, key, axis)
  * @param  value: Value of the input
  * @retval None
  */
static void Input_Manager_AddEvent(Input_Event_Type_t eventType, uint8_t deviceIndex, uint8_t inputId, int16_t value)
{
  /* Check if queue is full */
  if (eventCount >= MAX_INPUT_QUEUE_SIZE) {
    /* Queue is full, discard oldest event */
    queueHead = (queueHead + 1) % MAX_INPUT_QUEUE_SIZE;
    eventCount--;
  }
  
  /* Add event to queue */
  inputEventQueue[queueTail].eventType = eventType;
  inputEventQueue[queueTail].deviceIndex = deviceIndex;
  inputEventQueue[queueTail].inputId = inputId;
  inputEventQueue[queueTail].value = value;
  inputEventQueue[queueTail].timestamp = HAL_GetTick();
  
  /* Update queue tail */
  queueTail = (queueTail + 1) % MAX_INPUT_QUEUE_SIZE;
  eventCount++;
  
  /* Call user callback if registered */
  if (userCallback != NULL) {
    userCallback(&inputEventQueue[(queueTail - 1) % MAX_INPUT_QUEUE_SIZE]);
  }
}
