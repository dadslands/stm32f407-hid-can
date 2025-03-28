/**
 * @file display_manager.h
 * @brief Display Manager header file for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

#ifndef __DISPLAY_MANAGER_H
#define __DISPLAY_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  DISPLAY_ITEM_NONE = 0,
  DISPLAY_ITEM_TEXT,
  DISPLAY_ITEM_VALUE,
  DISPLAY_ITEM_BAR,
  DISPLAY_ITEM_ICON
} Display_Item_Type_t;

typedef enum {
  DISPLAY_DATA_SOURCE_NONE = 0,
  DISPLAY_DATA_SOURCE_SERIAL,
  DISPLAY_DATA_SOURCE_CAN,
  DISPLAY_DATA_SOURCE_SYSTEM
} Display_Data_Source_t;

typedef struct {
  uint8_t enabled;
  Display_Item_Type_t type;
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
  Display_Data_Source_t dataSource;
  union {
    struct {
      uint8_t dataIndex;
    } serial;
    struct {
      uint32_t canId;
      uint8_t dataIndex;
      uint8_t dataLength;
    } can;
    struct {
      uint8_t paramId;
    } system;
  } source;
  uint16_t color;
  uint16_t backgroundColor;
  char label[16];
  uint8_t refreshRate;
} Display_Item_t;

/* Exported constants --------------------------------------------------------*/
#define MAX_DISPLAY_ITEMS         16
#define DISPLAY_WIDTH             240
#define DISPLAY_HEIGHT            240

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void Display_Manager_Init(void);
void Display_Manager_Process(void);
uint8_t Display_Manager_AddItem(Display_Item_t* item);
uint8_t Display_Manager_RemoveItem(uint8_t itemIndex);
Display_Item_t* Display_Manager_GetItem(uint8_t itemIndex);
uint8_t Display_Manager_GetItemCount(void);
uint8_t Display_Manager_SaveConfig(void);
uint8_t Display_Manager_LoadConfig(void);
void Display_Manager_ResetConfig(void);
void Display_Manager_Clear(void);
void Display_Manager_DrawText(uint8_t x, uint8_t y, const char* text, uint16_t color);
void Display_Manager_DrawValue(uint8_t x, uint8_t y, int32_t value, uint16_t color);
void Display_Manager_DrawBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t value, uint16_t color);
void Display_Manager_DrawIcon(uint8_t x, uint8_t y, uint8_t iconId, uint16_t color);

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_MANAGER_H */
