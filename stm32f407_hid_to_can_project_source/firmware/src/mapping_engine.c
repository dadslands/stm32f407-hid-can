/**
 * @file mapping_engine.c
 * @brief Mapping Engine implementation for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

/* Includes ------------------------------------------------------------------*/
#include "mapping_engine.h"
#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "input_manager.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAPPING_CONFIG_ADDR       0x08060000  /* Flash sector for configuration storage */
#define MAPPING_CONFIG_SIZE       (sizeof(Input_Mapping_t) * MAX_MAPPINGS + 8)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Input_Mapping_t mappings[MAX_MAPPINGS];
uint8_t mappingCount = 0;

/* Private function prototypes -----------------------------------------------*/
static void Mapping_Engine_InputCallback(Input_Event_t* inputEvent);
static void Mapping_Engine_ProcessMapping(Input_Mapping_t* mapping, Input_Event_t* inputEvent);
static uint8_t Mapping_Engine_SendSerialOutput(Input_Mapping_t* mapping, int16_t value);
static uint8_t Mapping_Engine_SendCANOutput(Input_Mapping_t* mapping, int16_t value);

/* External variables --------------------------------------------------------*/
extern void Output_Manager_SendSerial(uint8_t* data, uint8_t length);
extern void Output_Manager_SendCAN(uint32_t canId, uint8_t* data, uint8_t length);

/**
  * @brief  Mapping Engine initialization function
  * @param  None
  * @retval None
  */
void Mapping_Engine_Init(void)
{
  /* Initialize mappings array */
  for (uint8_t i = 0; i < MAX_MAPPINGS; i++) {
    mappings[i].enabled = 0;
  }
  
  mappingCount = 0;
  
  /* Register callback for input events */
  Input_Manager_RegisterCallback(Mapping_Engine_InputCallback);
  
  /* Load configuration from flash */
  Mapping_Engine_LoadConfig();
}

/**
  * @brief  Mapping Engine process function, should be called periodically
  * @param  None
  * @retval None
  */
void Mapping_Engine_Process(void)
{
  /* Process input events */
  while (Input_Manager_GetEventCount() > 0) {
    Input_Event_t* event = Input_Manager_GetNextEvent();
    
    if (event != NULL) {
      /* Check all mappings for this event */
      for (uint8_t i = 0; i < MAX_MAPPINGS; i++) {
        if (mappings[i].enabled) {
          Mapping_Engine_ProcessMapping(&mappings[i], event);
        }
      }
    }
  }
}

/**
  * @brief  Add a new mapping
  * @param  mapping: Pointer to mapping structure
  * @retval uint8_t: Index of the new mapping, 0xFF if failed
  */
uint8_t Mapping_Engine_AddMapping(Input_Mapping_t* mapping)
{
  if (mapping == NULL) {
    return 0xFF;
  }
  
  /* Find an empty slot */
  for (uint8_t i = 0; i < MAX_MAPPINGS; i++) {
    if (!mappings[i].enabled) {
      /* Copy mapping to the slot */
      memcpy(&mappings[i], mapping, sizeof(Input_Mapping_t));
      mappings[i].enabled = 1;
      
      /* Update mapping count */
      mappingCount++;
      
      return i;
    }
  }
  
  /* No empty slot found */
  return 0xFF;
}

/**
  * @brief  Remove a mapping
  * @param  mappingIndex: Index of the mapping to remove
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Mapping_Engine_RemoveMapping(uint8_t mappingIndex)
{
  if (mappingIndex >= MAX_MAPPINGS || !mappings[mappingIndex].enabled) {
    return 0;
  }
  
  /* Disable the mapping */
  mappings[mappingIndex].enabled = 0;
  
  /* Update mapping count */
  mappingCount--;
  
  return 1;
}

/**
  * @brief  Get a mapping
  * @param  mappingIndex: Index of the mapping
  * @retval Input_Mapping_t*: Pointer to the mapping, NULL if not found
  */
Input_Mapping_t* Mapping_Engine_GetMapping(uint8_t mappingIndex)
{
  if (mappingIndex >= MAX_MAPPINGS || !mappings[mappingIndex].enabled) {
    return NULL;
  }
  
  return &mappings[mappingIndex];
}

/**
  * @brief  Get number of active mappings
  * @param  None
  * @retval uint8_t: Number of active mappings
  */
uint8_t Mapping_Engine_GetMappingCount(void)
{
  return mappingCount;
}

/**
  * @brief  Save mapping configuration to flash
  * @param  None
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Mapping_Engine_SaveConfig(void)
{
  /* In a real implementation, this would save the configuration to flash memory
     using the STM32 HAL flash functions. For this example, we'll just return success. */
  
  return 1;
}

/**
  * @brief  Load mapping configuration from flash
  * @param  None
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Mapping_Engine_LoadConfig(void)
{
  /* In a real implementation, this would load the configuration from flash memory
     using the STM32 HAL flash functions. For this example, we'll just return success. */
  
  return 1;
}

/**
  * @brief  Reset mapping configuration to defaults
  * @param  None
  * @retval None
  */
void Mapping_Engine_ResetConfig(void)
{
  /* Clear all mappings */
  for (uint8_t i = 0; i < MAX_MAPPINGS; i++) {
    mappings[i].enabled = 0;
  }
  
  mappingCount = 0;
  
  /* Add default mappings if needed */
  /* For example, map keyboard keys to serial output */
  Input_Mapping_t defaultMapping;
  
  /* Example: Map keyboard key 'A' to serial output */
  defaultMapping.enabled = 1;
  defaultMapping.deviceIndex = 0;  /* First keyboard */
  defaultMapping.eventType = INPUT_EVENT_KEY_PRESS;
  defaultMapping.inputId = 0x04;   /* 'A' key in HID usage table */
  defaultMapping.minValue = 0;
  defaultMapping.maxValue = 1;
  defaultMapping.outputType = OUTPUT_TYPE_SERIAL;
  defaultMapping.output.serial.dataFormat = 0;
  defaultMapping.output.serial.dataLength = 1;
  
  Mapping_Engine_AddMapping(&defaultMapping);
  
  /* Example: Map mouse movement to CAN output */
  defaultMapping.enabled = 1;
  defaultMapping.deviceIndex = 1;  /* First mouse */
  defaultMapping.eventType = INPUT_EVENT_AXIS_CHANGE;
  defaultMapping.inputId = 0;      /* X axis */
  defaultMapping.minValue = -127;
  defaultMapping.maxValue = 127;
  defaultMapping.outputType = OUTPUT_TYPE_CAN;
  defaultMapping.output.can.canId = 0x100;
  defaultMapping.output.can.dlc = 8;
  defaultMapping.output.can.dataIndex = 0;
  
  Mapping_Engine_AddMapping(&defaultMapping);
}

/**
  * @brief  Callback function for input events
  * @param  inputEvent: Pointer to input event structure
  * @retval None
  */
static void Mapping_Engine_InputCallback(Input_Event_t* inputEvent)
{
  /* This function is called directly from the Input Manager when a new event occurs.
     We don't need to do anything here since we process events in the main loop. */
}

/**
  * @brief  Process a mapping for an input event
  * @param  mapping: Pointer to mapping structure
  * @param  inputEvent: Pointer to input event structure
  * @retval None
  */
static void Mapping_Engine_ProcessMapping(Input_Mapping_t* mapping, Input_Event_t* inputEvent)
{
  /* Check if this mapping applies to this event */
  if (mapping->deviceIndex == inputEvent->deviceIndex &&
      mapping->eventType == inputEvent->eventType &&
      mapping->inputId == inputEvent->inputId) {
    
    /* Check if value is within range */
    if (inputEvent->value >= mapping->minValue && inputEvent->value <= mapping->maxValue) {
      /* Process based on output type */
      switch (mapping->outputType) {
        case OUTPUT_TYPE_SERIAL:
          Mapping_Engine_SendSerialOutput(mapping, inputEvent->value);
          break;
        
        case OUTPUT_TYPE_CAN:
          Mapping_Engine_SendCANOutput(mapping, inputEvent->value);
          break;
        
        default:
          break;
      }
    }
  }
}

/**
  * @brief  Send serial output for a mapping
  * @param  mapping: Pointer to mapping structure
  * @param  value: Input value
  * @retval uint8_t: 1 if successful, 0 if failed
  */
static uint8_t Mapping_Engine_SendSerialOutput(Input_Mapping_t* mapping, int16_t value)
{
  /* Prepare data for serial output */
  uint8_t data[8] = {0};
  uint8_t length = mapping->output.serial.dataLength;
  
  /* Format data based on data format */
  switch (mapping->output.serial.dataFormat) {
    case 0:  /* Raw value */
      data[0] = (uint8_t)value;
      break;
    
    case 1:  /* ASCII */
      data[0] = (uint8_t)value + 0x30;  /* Convert to ASCII */
      break;
    
    case 2:  /* Binary (LSB first) */
      data[0] = (uint8_t)(value & 0xFF);
      data[1] = (uint8_t)((value >> 8) & 0xFF);
      break;
    
    case 3:  /* Binary (MSB first) */
      data[0] = (uint8_t)((value >> 8) & 0xFF);
      data[1] = (uint8_t)(value & 0xFF);
      break;
    
    default:
      data[0] = (uint8_t)value;
      break;
  }
  
  /* Send data to Output Manager */
  Output_Manager_SendSerial(data, length);
  
  return 1;
}

/**
  * @brief  Send CAN output for a mapping
  * @param  mapping: Pointer to mapping structure
  * @param  value: Input value
  * @retval uint8_t: 1 if successful, 0 if failed
  */
static uint8_t Mapping_Engine_SendCANOutput(Input_Mapping_t* mapping, int16_t value)
{
  /* Prepare data for CAN output */
  uint8_t data[8] = {0};
  uint8_t length = mapping->output.can.dlc;
  uint8_t dataIndex = mapping->output.can.dataIndex;
  
  /* Ensure data index is valid */
  if (dataIndex >= length) {
    return 0;
  }
  
  /* Place value in data array */
  if (dataIndex + 1 < length) {
    /* 16-bit value */
    data[dataIndex] = (uint8_t)(value & 0xFF);
    data[dataIndex + 1] = (uint8_t)((value >> 8) & 0xFF);
  } else {
    /* 8-bit value */
    data[dataIndex] = (uint8_t)value;
  }
  
  /* Send data to Output Manager */
  Output_Manager_SendCAN(mapping->output.can.canId, data, length);
  
  return 1;
}
