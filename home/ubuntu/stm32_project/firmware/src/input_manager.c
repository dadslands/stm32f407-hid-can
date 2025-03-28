/**
 * @file input_manager.c
 * @brief Input Manager implementation for handling HID device inputs
 * @author Manus AI
 * @date March 2025
 * 
 * This file implements the Input Manager module that processes input events
 * from HID devices connected via USB. It normalizes inputs from different
 * device types (keyboards, mice, gamepads) into a common event format
 * that can be used by the Mapping Engine.
 * 
 * The implementation includes event queue management, device state tracking,
 * and input event processing logic.
 * 
 * References:
 * - USB HID Usage Tables: https://www.usb.org/document-library/hid-usage-tables-112
 * - USB HID Specification: https://www.usb.org/document-library/device-class-definition-hid-111
 * - STM32 USB Host Library: https://www.st.com/resource/en/user_manual/dm00108129.pdf
 */

#include "input_manager.h"
#include "usb_host.h"
#include "main.h"
#include <string.h>

/* Private defines */
#define KEYBOARD_USAGE_PAGE 0x01
#define MOUSE_USAGE_PAGE    0x01
#define JOYSTICK_USAGE_PAGE 0x01

#define KEYBOARD_USAGE      0x06
#define MOUSE_USAGE         0x02
#define JOYSTICK_USAGE      0x04

/* Private types */
typedef struct {
    InputEvent_t events[INPUT_EVENT_QUEUE_SIZE]; /* Circular buffer for events */
    uint16_t head;                              /* Index of the next event to read */
    uint16_t tail;                              /* Index of the next slot to write */
    uint16_t count;                             /* Number of events in the queue */
} InputEventQueue_t;

typedef struct {
    bool keyState[256];                         /* Current state of each key (true=pressed) */
    uint16_t pressedKeys[MAX_SIMULTANEOUS_KEYS]; /* List of currently pressed keys */
    uint8_t pressedKeyCount;                    /* Number of currently pressed keys */
    
    int32_t mouseX;                             /* Current mouse X position */
    int32_t mouseY;                             /* Current mouse Y position */
    bool mouseButtons[3];                       /* Current state of mouse buttons (0=left, 1=right, 2=middle) */
    int8_t mouseWheel;                          /* Current mouse wheel position */
    
    int32_t joystickAxes[8];                    /* Current joystick axis positions */
    bool joystickButtons[32];                   /* Current state of joystick buttons */
} InputDeviceState_t;

/* Private variables */
static InputEventQueue_t eventQueue;
static InputDeviceState_t deviceStates[MAX_HID_DEVICES];
static void (*eventCallback)(InputEvent_t *event) = NULL;

/* Private function prototypes */
static void USB_HOST_ConnectionCallback(uint8_t deviceId, HID_DeviceState_t state);
static void USB_HOST_DataCallback(uint8_t deviceId, uint8_t *data, uint16_t length);
static void ProcessKeyboardData(uint8_t deviceId, uint8_t *data, uint16_t length);
static void ProcessMouseData(uint8_t deviceId, uint8_t *data, uint16_t length);
static void ProcessJoystickData(uint8_t deviceId, uint8_t *data, uint16_t length);
static bool EnqueueEvent(InputEvent_t *event);
static bool DequeueEvent(InputEvent_t *event);

/**
 * @brief Initialize the Input Manager
 * 
 * This function initializes the Input Manager module, setting up
 * the event queue and registering callbacks with the USB Host.
 * 
 * The initialization process includes:
 * 1. Initializing the event queue
 * 2. Initializing device state tracking
 * 3. Registering callbacks with the USB Host
 * 
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef InputManager_Init(void)
{
    DEBUG_PRINT("Input Manager: Initializing...\r\n");
    
    /* Initialize event queue */
    eventQueue.head = 0;
    eventQueue.tail = 0;
    eventQueue.count = 0;
    
    /* Initialize device states */
    for (uint8_t i = 0; i < MAX_HID_DEVICES; i++) {
        memset(&deviceStates[i], 0, sizeof(InputDeviceState_t));
    }
    
    /* Register callbacks with USB Host */
    USB_HOST_RegisterConnectionCallback(USB_HOST_ConnectionCallback);
    USB_HOST_RegisterDataCallback(USB_HOST_DataCallback);
    
    DEBUG_PRINT("Input Manager: Initialized successfully\r\n");
    return HAL_OK;
}

/**
 * @brief Get the next input event from the queue
 * 
 * This function retrieves the next input event from the event queue.
 * If the queue is empty, it returns false.
 * 
 * The function is non-blocking and should be called regularly in the
 * main loop to process input events.
 * 
 * @param event Pointer to store the retrieved event
 * @return bool true if an event was retrieved, false otherwise
 */
bool InputManager_GetNextEvent(InputEvent_t *event)
{
    if (event == NULL) {
        return false;
    }
    
    return DequeueEvent(event);
}

/**
 * @brief Get the current state of a specific key
 * 
 * This function returns the current state of a specific key
 * (pressed or released). It checks all connected keyboard devices.
 * 
 * @param keyCode Key code to check (USB HID key code)
 * @return bool true if the key is pressed, false otherwise
 */
bool InputManager_IsKeyPressed(uint16_t keyCode)
{
    if (keyCode >= 256) {
        return false;
    }
    
    /* Check all devices for the key state */
    for (uint8_t i = 0; i < MAX_HID_DEVICES; i++) {
        if (deviceStates[i].keyState[keyCode]) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Get the current mouse position
 * 
 * This function returns the current mouse position as X and Y coordinates.
 * If multiple mice are connected, it returns the position of the first mouse.
 * 
 * @param x Pointer to store X coordinate
 * @param y Pointer to store Y coordinate
 * @return None
 */
void InputManager_GetMousePosition(int32_t *x, int32_t *y)
{
    if (x == NULL || y == NULL) {
        return;
    }
    
    /* Find the first connected mouse device */
    for (uint8_t i = 0; i < MAX_HID_DEVICES; i++) {
        HID_DeviceInfo_t deviceInfo;
        if (USB_HOST_GetDeviceInfo(i, &deviceInfo) == HAL_OK) {
            if (deviceInfo.type == HID_DEVICE_MOUSE) {
                *x = deviceStates[i].mouseX;
                *y = deviceStates[i].mouseY;
                return;
            }
        }
    }
    
    /* No mouse found, return zeros */
    *x = 0;
    *y = 0;
}

/**
 * @brief Get the current state of a mouse button
 * 
 * This function returns the current state of a specific mouse button
 * (pressed or released). If multiple mice are connected, it checks
 * all of them and returns true if any has the button pressed.
 * 
 * @param button Button ID to check (0=left, 1=right, 2=middle)
 * @return bool true if the button is pressed, false otherwise
 */
bool InputManager_IsMouseButtonPressed(uint8_t button)
{
    if (button >= 3) {
        return false;
    }
    
    /* Check all devices for the button state */
    for (uint8_t i = 0; i < MAX_HID_DEVICES; i++) {
        HID_DeviceInfo_t deviceInfo;
        if (USB_HOST_GetDeviceInfo(i, &deviceInfo) == HAL_OK) {
            if (deviceInfo.type == HID_DEVICE_MOUSE) {
                if (deviceStates[i].mouseButtons[button]) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

/**
 * @brief Get the current position of a joystick axis
 * 
 * This function returns the current position of a specific joystick axis.
 * 
 * @param deviceId Device ID of the joystick
 * @param axis Axis ID to check (0=X, 1=Y, 2=Z, etc.)
 * @return int32_t Axis position (-32768 to 32767)
 */
int32_t InputManager_GetJoystickAxis(uint8_t deviceId, uint8_t axis)
{
    if (deviceId >= MAX_HID_DEVICES || axis >= 8) {
        return 0;
    }
    
    HID_DeviceInfo_t deviceInfo;
    if (USB_HOST_GetDeviceInfo(deviceId, &deviceInfo) != HAL_OK) {
        return 0;
    }
    
    if (deviceInfo.type != HID_DEVICE_JOYSTICK && deviceInfo.type != HID_DEVICE_GAMEPAD) {
        return 0;
    }
    
    return deviceStates[deviceId].joystickAxes[axis];
}

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
bool InputManager_IsJoystickButtonPressed(uint8_t deviceId, uint8_t button)
{
    if (deviceId >= MAX_HID_DEVICES || button >= 32) {
        return false;
    }
    
    HID_DeviceInfo_t deviceInfo;
    if (USB_HOST_GetDeviceInfo(deviceId, &deviceInfo) != HAL_OK) {
        return false;
    }
    
    if (deviceInfo.type != HID_DEVICE_JOYSTICK && deviceInfo.type != HID_DEVICE_GAMEPAD) {
        return false;
    }
    
    return deviceStates[deviceId].joystickButtons[button];
}

/**
 * @brief Register a callback for input events
 * 
 * This function registers a callback that will be called when
 * a new input event is processed. This allows for immediate
 * event handling without polling the event queue.
 * 
 * @param callback Function pointer to the callback
 * @return None
 */
void InputManager_RegisterEventCallback(void (*callback)(InputEvent_t *event))
{
    eventCallback = callback;
}

/**
 * @brief Get the number of connected input devices
 * 
 * This function returns the number of currently connected input devices.
 * 
 * @return uint8_t Number of connected devices
 */
uint8_t InputManager_GetDeviceCount(void)
{
    return USB_HOST_GetConnectedDeviceCount();
}

/**
 * @brief Get information about a connected input device
 * 
 * This function retrieves information about a connected input device
 * based on its ID.
 * 
 * @param deviceId Device ID to query
 * @param deviceName Buffer to store device name
 * @param deviceType Pointer to store device type
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef InputManager_GetDeviceInfo(uint8_t deviceId, char *deviceName, uint8_t *deviceType)
{
    if (deviceId >= MAX_HID_DEVICES || deviceName == NULL || deviceType == NULL) {
        return HAL_ERROR;
    }
    
    HID_DeviceInfo_t deviceInfo;
    if (USB_HOST_GetDeviceInfo(deviceId, &deviceInfo) != HAL_OK) {
        return HAL_ERROR;
    }
    
    /* Copy device name */
    strcpy(deviceName, deviceInfo.product);
    
    /* Copy device type */
    *deviceType = deviceInfo.type;
    
    return HAL_OK;
}

/**
 * @brief Reset the Input Manager
 * 
 * This function resets the Input Manager, clearing the event queue
 * and resetting all device states.
 * 
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef InputManager_Reset(void)
{
    DEBUG_PRINT("Input Manager: Resetting...\r\n");
    
    /* Clear event queue */
    eventQueue.head = 0;
    eventQueue.tail = 0;
    eventQueue.count = 0;
    
    /* Reset device states */
    for (uint8_t i = 0; i < MAX_HID_DEVICES; i++) {
        memset(&deviceStates[i], 0, sizeof(InputDeviceState_t));
    }
    
    DEBUG_PRINT("Input Manager: Reset complete\r\n");
    return HAL_OK;
}

/**
 * @brief Debug function to print Input Manager status
 * 
 * This function prints detailed information about the Input Manager
 * status, including connected devices and current input states.
 * 
 * @return None
 */
void InputManager_DebugPrint(void)
{
    DEBUG_PRINT("Input Manager Status:\r\n");
    DEBUG_PRINT("  Event queue: %d/%d events\r\n", eventQueue.count, INPUT_EVENT_QUEUE_SIZE);
    
    /* Print device information */
    for (uint8_t i = 0; i < MAX_HID_DEVICES; i++) {
        HID_DeviceInfo_t deviceInfo;
        if (USB_HOST_GetDeviceInfo(i, &deviceInfo) == HAL_OK) {
            DEBUG_PRINT("  Device %d (%s):\r\n", i, deviceInfo.product);
            
            /* Print device-specific information based on type */
            switch (deviceInfo.type) {
                case HID_DEVICE_KEYBOARD:
                    DEBUG_PRINT("    Type: Keyboard\r\n");
                    DEBUG_PRINT("    Pressed keys: %d\r\n", deviceStates[i].pressedKeyCount);
                    for (uint8_t j = 0; j < deviceStates[i].pressedKeyCount; j++) {
                        DEBUG_PRINT("      Key 0x%02X\r\n", deviceStates[i].pressedKeys[j]);
                    }
                    break;
                    
                case HID_DEVICE_MOUSE:
                    DEBUG_PRINT("    Type: Mouse\r\n");
                    DEBUG_PRINT("    Position: X=%ld, Y=%ld\r\n", deviceStates[i].mouseX, deviceStates[i].mouseY);
                    DEBUG_PRINT("    Buttons: L=%d, R=%d, M=%d\r\n", 
                               deviceStates[i].mouseButtons[0],
                               deviceStates[i].mouseButtons[1],
                               deviceStates[i].mouseButtons[2]);
                    DEBUG_PRINT("    Wheel: %d\r\n", deviceStates[i].mouseWheel);
                    break;
                    
                case HID_DEVICE_JOYSTICK:
                case HID_DEVICE_GAMEPAD:
                    DEBUG_PRINT("    Type: %s\r\n", deviceInfo.type == HID_DEVICE_JOYSTICK ? "Joystick" : "Gamepad");
                    DEBUG_PRINT("    Axes: X=%ld, Y=%ld, Z=%ld\r\n", 
                               deviceStates[i].joystickAxes[0],
                               deviceStates[i].joystickAxes[1],
                               deviceStates[i].joystickAxes[2]);
                    
                    DEBUG_PRINT("    Buttons: ");
                    for (uint8_t j = 0; j < 8; j++) {
                        DEBUG_PRINT("%d", deviceStates[i].joystickButtons[j] ? 1 : 0);
                    }
                    DEBUG_PRINT("\r\n");
                    break;
                    
                default:
                    DEBUG_PRINT("    Type: Unknown\r\n");
                    break;
            }
        }
    }
}

/**
 * @brief USB Host connection callback
 * 
 * This function is called by the USB Host when a device is connected
 * or disconnected. It updates the device state tracking.
 * 
 * @param deviceId Device ID that changed state
 * @param state New device state
 * @return None
 */
static void USB_HOST_ConnectionCallback(uint8_t deviceId, HID_DeviceState_t state)
{
    if (deviceId >= MAX_HID_DEVICES) {
        return;
    }
    
    if (state == HID_DEVICE_READY) {
        DEBUG_PRINT("Input Manager: Device %d connected and ready\r\n", deviceId);
        
        /* Reset device state */
        memset(&deviceStates[deviceId], 0, sizeof(InputDeviceState_t));
        
        /* Get device information */
        HID_DeviceInfo_t deviceInfo;
        if (USB_HOST_GetDeviceInfo(deviceId, &deviceInfo) == HAL_OK) {
            DEBUG_PRINT("Input Manager: Device type: %d\r\n", deviceInfo.type);
        }
    } else if (state == HID_DEVICE_DISCONNECTED) {
        DEBUG_PRINT("Input Manager: Device %d disconnected\r\n", deviceId);
        
        /* Reset device state */
        memset(&deviceStates[deviceId], 0, sizeof(InputDeviceState_t));
    }
}

/**
 * @brief USB Host data callback
 * 
 * This function is called by the USB Host when data is received from
 * a device. It processes the data based on the device type and
 * generates input events.
 * 
 * @param deviceId Device ID that sent the data
 * @param data Pointer to the received data
 * @param length Length of the received data
 * @return None
 */
static void USB_HOST_DataCallback(uint8_t deviceId, uint8_t *data, uint16_t length)
{
    if (deviceId >= MAX_HID_DEVICES || data == NULL || length == 0) {
        return;
    }
    
    /* Get device information */
    HID_DeviceInfo_t deviceInfo;
    if (USB_HOST_GetDeviceInfo(deviceId, &deviceInfo) != HAL_OK) {
        return;
    }
    
    /* Process data based on device type */
    switch (deviceInfo.type) {
        case HID_DEVICE_KEYBOARD:
            ProcessKeyboardData(deviceId, data, length);
            break;
            
        case HID_DEVICE_MOUSE:
            ProcessMouseData(deviceId, data, length);
            break;
            
        case HID_DEVICE_JOYSTICK:
        case HID_DEVICE<response clipped><NOTE>To save on context only part of this file has been shown to you. You should retry this tool after you have searched inside the file with `grep -n` in order to find the line numbers of what you are looking for.</NOTE>