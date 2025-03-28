/**
 * @file input_manager.h
 * @brief Input Manager for handling HID device inputs
 * @author Manus AI
 * @date March 2025
 * 
 * This file contains the declarations for the Input Manager module
 * that processes input events from HID devices connected via USB.
 * It provides an abstraction layer between the USB Host and the
 * Mapping Engine, normalizing input events from different device types.
 * 
 * References:
 * - USB HID Usage Tables: https://www.usb.org/document-library/hid-usage-tables-112
 * - USB HID Specification: https://www.usb.org/document-library/device-class-definition-hid-111
 * - STM32 USB Host Library: https://www.st.com/resource/en/user_manual/dm00108129.pdf
 */

#ifndef __INPUT_MANAGER_H
#define __INPUT_MANAGER_H

#include "main.h"
#include <stdint.h>

/* Maximum number of events in the input event queue */
#define INPUT_EVENT_QUEUE_SIZE 32

/* Maximum number of simultaneous key presses to track */
#define MAX_SIMULTANEOUS_KEYS 10

/* Input event types */
typedef enum {
  INPUT_EVENT_NONE = 0,
  INPUT_EVENT_KEY_DOWN,
  INPUT_EVENT_KEY_UP,
  INPUT_EVENT_MOUSE_MOVE,
  INPUT_EVENT_MOUSE_BUTTON,
  INPUT_EVENT_MOUSE_WHEEL,
  INPUT_EVENT_JOYSTICK_MOVE,
  INPUT_EVENT_JOYSTICK_BUTTON
} InputEventType_t;

/* Input event structure */
typedef struct {
  InputEventType_t type;    /* Type of event */
  uint8_t deviceId;         /* Source device ID */
  uint16_t code;            /* Event code (key code, button ID, axis ID) */
  int32_t value;            /* Event value */
  uint32_t timestamp;       /* Event timestamp (ms) */
} InputEvent_t;

/**
 * @brief Initialize the Input Manager
 * 
 * This function initializes the Input Manager module, setting up
 * the event queue and registering callbacks with the USB Host.
 * 
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef InputManager_Init(void);

/**
 * @brief Get the next input event from the queue
 * 
 * This function retrieves the next input event from the event queue.
 * If the queue is empty, it returns false.
 * 
 * @param event Pointer to store the retrieved event
 * @return bool true if an event was retrieved, false otherwise
 */
bool InputManager_GetNextEvent(InputEvent_t *event);

/**
 * @brief Get the current state of a specific key
 * 
 * This function returns the current state of a specific key
 * (pressed or released).
 * 
 * @param keyCode Key code to check
 * @return bool true if the key is pressed, false otherwise
 */
bool InputManager_IsKeyPressed(uint16_t keyCode);

/**
 * @brief Get the current mouse position
 * 
 * This function returns the current mouse position as X and Y coordinates.
 * 
 * @param x Pointer to store X coordinate
 * @param y Pointer to store Y coordinate
 * @return None
 */
void InputManager_GetMousePosition(int32_t *x, int32_t *y);

/**
 * @brief Get the current state of a mouse button
 * 
 * This function returns the current state of a specific mouse button
 * (pressed or released).
 * 
 * @param button Button ID to check (0=left, 1=right, 2=middle)
 * @return bool true if the button is pressed, false otherwise
 */
bool InputManager_IsMouseButtonPressed(uint8_t button);

/**
 * @brief Get the current position of a joystick axis
 * 
 * This function returns the current position of a specific joystick axis.
 * 
 * @param deviceId Device ID of the joystick
 * @param axis Axis ID to check (0=X, 1=Y, 2=Z, etc.)
 * @return int32_t Axis position (-32768 to 32767)
 */
int32_t InputManager_GetJoystickAxis(uint8_t deviceId, uint8_t axis);

/**
 * @brief Get the current state of a joystick button
 * 
 * This function returns the current state of a specific joystick button
 * (pressed or released).
 * 
 * @param deviceId Device ID of the joystick
 * @param button Button ID to check
 * @return bool true if the button is pressed, false otherwise
 */
bool InputManager_IsJoystickButtonPressed(uint8_t deviceId, uint8_t button);

/**
 * @brief Register a callback for input events
 * 
 * This function registers a callback that will be called when
 * a new input event is processed.
 * 
 * @param callback Function pointer to the callback
 * @return None
 */
void InputManager_RegisterEventCallback(void (*callback)(InputEvent_t *event));

/**
 * @brief Get the number of connected input devices
 * 
 * This function returns the number of currently connected input devices.
 * 
 * @return uint8_t Number of connected devices
 */
uint8_t InputManager_GetDeviceCount(void);

/**
 * @brief Get information about a connected input device
 * 
 * This function retrieves information about a connected input device
 * based on its ID.
 * 
 * @param deviceId Device ID to query
 * @param deviceName Buffer to store device name
 * @param deviceType Pointer to store device type
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef InputManager_GetDeviceInfo(uint8_t deviceId, char *deviceName, uint8_t *deviceType);

/**
 * @brief Reset the Input Manager
 * 
 * This function resets the Input Manager, clearing the event queue
 * and resetting all device states.
 * 
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef InputManager_Reset(void);

/**
 * @brief Debug function to print Input Manager status
 * 
 * This function prints detailed information about the Input Manager
 * status, including connected devices and current input states.
 * 
 * @return None
 */
void InputManager_DebugPrint(void);

#endif /* __INPUT_MANAGER_H */
