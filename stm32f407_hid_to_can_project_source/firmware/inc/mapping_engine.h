/**
 * @file mapping_engine.h
 * @brief Mapping Engine header file for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

#ifndef __MAPPING_ENGINE_H
#define __MAPPING_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "input_manager.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  OUTPUT_TYPE_NONE = 0,
  OUTPUT_TYPE_SERIAL,
  OUTPUT_TYPE_CAN
} Output_Type_t;

typedef struct {
  uint8_t enabled;
  uint8_t deviceIndex;
  Input_Event_Type_t eventType;
  uint8_t inputId;
  int16_t minValue;
  int16_t maxValue;
  Output_Type_t outputType;
  union {
    struct {
      uint8_t dataFormat;
      uint8_t dataLength;
    } serial;
    struct {
      uint32_t canId;
      uint8_t dlc;
      uint8_t dataIndex;
    } can;
  } output;
} Input_Mapping_t;

/* Exported constants --------------------------------------------------------*/
#define MAX_MAPPINGS              64
#define MAPPING_CONFIG_VERSION    1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void Mapping_Engine_Init(void);
void Mapping_Engine_Process(void);
uint8_t Mapping_Engine_AddMapping(Input_Mapping_t* mapping);
uint8_t Mapping_Engine_RemoveMapping(uint8_t mappingIndex);
Input_Mapping_t* Mapping_Engine_GetMapping(uint8_t mappingIndex);
uint8_t Mapping_Engine_GetMappingCount(void);
uint8_t Mapping_Engine_SaveConfig(void);
uint8_t Mapping_Engine_LoadConfig(void);
void Mapping_Engine_ResetConfig(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAPPING_ENGINE_H */
