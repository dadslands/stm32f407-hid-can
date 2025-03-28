/**
 * @file display_manager.c
 * @brief Display Manager implementation for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

/* Includes ------------------------------------------------------------------*/
#include "display_manager.h"
#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DISPLAY_CONFIG_ADDR       0x08080000  /* Flash sector for configuration storage */
#define DISPLAY_CONFIG_SIZE       (sizeof(Display_Item_t) * MAX_DISPLAY_ITEMS + 8)

/* GC9A01 Commands */
#define GC9A01_SWRESET            0x01
#define GC9A01_SLPIN              0x10
#define GC9A01_SLPOUT             0x11
#define GC9A01_INVOFF             0x20
#define GC9A01_INVON              0x21
#define GC9A01_DISPOFF            0x28
#define GC9A01_DISPON             0x29
#define GC9A01_CASET              0x2A
#define GC9A01_RASET              0x2B
#define GC9A01_RAMWR              0x2C
#define GC9A01_MADCTL             0x36
#define GC9A01_COLMOD             0x3A

/* Color definitions */
#define COLOR_BLACK               0x0000
#define COLOR_WHITE               0xFFFF
#define COLOR_RED                 0xF800
#define COLOR_GREEN               0x07E0
#define COLOR_BLUE                0x001F
#define COLOR_YELLOW              0xFFE0
#define COLOR_CYAN                0x07FF
#define COLOR_MAGENTA             0xF81F

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
Display_Item_t displayItems[MAX_DISPLAY_ITEMS];
uint8_t displayItemCount = 0;
uint8_t displayBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT * 2];  /* 16-bit color, 2 bytes per pixel */

/* Private function prototypes -----------------------------------------------*/
static void Display_Manager_InitGC9A01(void);
static void Display_Manager_WriteCommand(uint8_t cmd);
static void Display_Manager_WriteData(uint8_t data);
static void Display_Manager_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void Display_Manager_UpdateItem(Display_Item_t* item);
static uint32_t Display_Manager_GetItemValue(Display_Item_t* item);

/* External variables --------------------------------------------------------*/

/**
  * @brief  Display Manager initialization function
  * @param  None
  * @retval None
  */
void Display_Manager_Init(void)
{
  /* Initialize SPI for display */
  hspi1.Instance = DISPLAY_SPI;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    Error_Handler();
  }
  
  /* Initialize GC9A01 display controller */
  Display_Manager_InitGC9A01();
  
  /* Initialize display items */
  for (uint8_t i = 0; i < MAX_DISPLAY_ITEMS; i++) {
    displayItems[i].enabled = 0;
  }
  
  displayItemCount = 0;
  
  /* Load configuration from flash */
  Display_Manager_LoadConfig();
  
  /* Clear display */
  Display_Manager_Clear();
}

/**
  * @brief  Display Manager process function, should be called periodically
  * @param  None
  * @retval None
  */
void Display_Manager_Process(void)
{
  static uint32_t lastUpdateTime = 0;
  uint32_t currentTime = HAL_GetTick();
  
  /* Update display items based on their refresh rate */
  for (uint8_t i = 0; i < MAX_DISPLAY_ITEMS; i++) {
    if (displayItems[i].enabled) {
      /* Check if it's time to update this item */
      if (currentTime - lastUpdateTime >= displayItems[i].refreshRate) {
        Display_Manager_UpdateItem(&displayItems[i]);
      }
    }
  }
  
  /* Update last update time if any item was updated */
  if (currentTime - lastUpdateTime >= 100) {  /* Minimum update interval */
    lastUpdateTime = currentTime;
  }
}

/**
  * @brief  Add a new display item
  * @param  item: Pointer to display item structure
  * @retval uint8_t: Index of the new item, 0xFF if failed
  */
uint8_t Display_Manager_AddItem(Display_Item_t* item)
{
  if (item == NULL) {
    return 0xFF;
  }
  
  /* Find an empty slot */
  for (uint8_t i = 0; i < MAX_DISPLAY_ITEMS; i++) {
    if (!displayItems[i].enabled) {
      /* Copy item to the slot */
      memcpy(&displayItems[i], item, sizeof(Display_Item_t));
      displayItems[i].enabled = 1;
      
      /* Update item count */
      displayItemCount++;
      
      return i;
    }
  }
  
  /* No empty slot found */
  return 0xFF;
}

/**
  * @brief  Remove a display item
  * @param  itemIndex: Index of the item to remove
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Display_Manager_RemoveItem(uint8_t itemIndex)
{
  if (itemIndex >= MAX_DISPLAY_ITEMS || !displayItems[itemIndex].enabled) {
    return 0;
  }
  
  /* Disable the item */
  displayItems[itemIndex].enabled = 0;
  
  /* Update item count */
  displayItemCount--;
  
  return 1;
}

/**
  * @brief  Get a display item
  * @param  itemIndex: Index of the item
  * @retval Display_Item_t*: Pointer to the item, NULL if not found
  */
Display_Item_t* Display_Manager_GetItem(uint8_t itemIndex)
{
  if (itemIndex >= MAX_DISPLAY_ITEMS || !displayItems[itemIndex].enabled) {
    return NULL;
  }
  
  return &displayItems[itemIndex];
}

/**
  * @brief  Get number of active display items
  * @param  None
  * @retval uint8_t: Number of active items
  */
uint8_t Display_Manager_GetItemCount(void)
{
  return displayItemCount;
}

/**
  * @brief  Save display configuration to flash
  * @param  None
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Display_Manager_SaveConfig(void)
{
  /* In a real implementation, this would save the configuration to flash memory
     using the STM32 HAL flash functions. For this example, we'll just return success. */
  
  return 1;
}

/**
  * @brief  Load display configuration from flash
  * @param  None
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Display_Manager_LoadConfig(void)
{
  /* In a real implementation, this would load the configuration from flash memory
     using the STM32 HAL flash functions. For this example, we'll set default values. */
  
  /* Add default display items */
  Display_Manager_ResetConfig();
  
  return 1;
}

/**
  * @brief  Reset display configuration to defaults
  * @param  None
  * @retval None
  */
void Display_Manager_ResetConfig(void)
{
  /* Clear all items */
  for (uint8_t i = 0; i < MAX_DISPLAY_ITEMS; i++) {
    displayItems[i].enabled = 0;
  }
  
  displayItemCount = 0;
  
  /* Add default items */
  
  /* Example: System status text */
  Display_Item_t defaultItem;
  defaultItem.enabled = 1;
  defaultItem.type = DISPLAY_ITEM_TEXT;
  defaultItem.x = 80;
  defaultItem.y = 100;
  defaultItem.width = 80;
  defaultItem.height = 20;
  defaultItem.dataSource = DISPLAY_DATA_SOURCE_SYSTEM;
  defaultItem.source.system.paramId = 0;  /* System status */
  defaultItem.color = COLOR_WHITE;
  defaultItem.backgroundColor = COLOR_BLACK;
  strcpy(defaultItem.label, "Status");
  defaultItem.refreshRate = 1000;  /* Update every second */
  
  Display_Manager_AddItem(&defaultItem);
  
  /* Example: CAN data value */
  defaultItem.enabled = 1;
  defaultItem.type = DISPLAY_ITEM_VALUE;
  defaultItem.x = 80;
  defaultItem.y = 140;
  defaultItem.width = 80;
  defaultItem.height = 20;
  defaultItem.dataSource = DISPLAY_DATA_SOURCE_CAN;
  defaultItem.source.can.canId = 0x100;
  defaultItem.source.can.dataIndex = 0;
  defaultItem.source.can.dataLength = 2;
  defaultItem.color = COLOR_GREEN;
  defaultItem.backgroundColor = COLOR_BLACK;
  strcpy(defaultItem.label, "RPM");
  defaultItem.refreshRate = 100;  /* Update 10 times per second */
  
  Display_Manager_AddItem(&defaultItem);
}

/**
  * @brief  Clear display
  * @param  None
  * @retval None
  */
void Display_Manager_Clear(void)
{
  /* Set window to entire display */
  Display_Manager_SetWindow(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
  
  /* Fill with black */
  Display_Manager_WriteCommand(GC9A01_RAMWR);
  
  for (uint32_t i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
    Display_Manager_WriteData(0x00);  /* Black, high byte */
    Display_Manager_WriteData(0x00);  /* Black, low byte */
  }
}

/**
  * @brief  Draw text on display
  * @param  x: X coordinate
  * @param  y: Y coordinate
  * @param  text: Text to draw
  * @param  color: Text color
  * @retval None
  */
void Display_Manager_DrawText(uint8_t x, uint8_t y, const char* text, uint16_t color)
{
  /* In a real implementation, this would use a font library to draw text.
     For this example, we'll just set a window and fill it with the text color. */
  
  uint8_t width = strlen(text) * 8;  /* Assume 8 pixels per character */
  uint8_t height = 16;  /* Assume 16 pixels height */
  
  /* Set window */
  Display_Manager_SetWindow(x, y, x + width - 1, y + height - 1);
  
  /* Fill with color */
  Display_Manager_WriteCommand(GC9A01_RAMWR);
  
  for (uint32_t i = 0; i < width * height; i++) {
    Display_Manager_WriteData((color >> 8) & 0xFF);  /* High byte */
    Display_Manager_WriteData(color & 0xFF);  /* Low byte */
  }
}

/**
  * @brief  Draw value on display
  * @param  x: X coordinate
  * @param  y: Y coordinate
  * @param  value: Value to draw
  * @param  color: Value color
  * @retval None
  */
void Display_Manager_DrawValue(uint8_t x, uint8_t y, int32_t value, uint16_t color)
{
  /* Convert value to string */
  char valueStr[16];
  sprintf(valueStr, "%ld", value);
  
  /* Draw text */
  Display_Manager_DrawText(x, y, valueStr, color);
}

/**
  * @brief  Draw bar on display
  * @param  x: X coordinate
  * @param  y: Y coordinate
  * @param  width: Bar width
  * @param  height: Bar height
  * @param  value: Bar value (0-100)
  * @param  color: Bar color
  * @retval None
  */
void Display_Manager_DrawBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t value, uint16_t color)
{
  /* Limit value to 0-100 */
  if (value > 100) {
    value = 100;
  }
  
  /* Calculate bar width based on value */
  uint8_t barWidth = (width * value) / 100;
  
  /* Set window for bar */
  Display_Manager_SetWindow(x, y, x + barWidth - 1, y + height - 1);
  
  /* Fill with color */
  Display_Manager_WriteCommand(GC9A01_RAMWR);
  
  for (uint32_t i = 0; i < barWidth * height; i++) {
    Display_Manager_WriteData((color >> 8) & 0xFF);  /* High byte */
    Display_Manager_WriteData(color & 0xFF);  /* Low byte */
  }
  
  /* Set window for background */
  if (barWidth < width) {
    Display_Manager_SetWindow(x + barWidth, y, x + width - 1, y + height - 1);
    
    /* Fill with black */
    Display_Manager_WriteCommand(GC9A01_RAMWR);
    
    for (uint32_t i = 0; i < (width - barWidth) * height; i++) {
      Display_Manager_WriteData(0x00);  /* Black, high byte */
      Display_Manager_WriteData(0x00);  /* Black, low byte */
    }
  }
}

/**
  * @brief  Draw icon on display
  * @param  x: X coordinate
  * @param  y: Y coordinate
  * @param  iconId: Icon ID
  * @param  color: Icon color
  * @retval None
  */
void Display_Manager_DrawIcon(uint8_t x, uint8_t y, uint8_t iconId, uint16_t color)
{
  /* In a real implementation, this would use an icon library.
     For this example, we'll just draw a simple shape based on iconId. */
  
  uint8_t size = 16;  /* Icon size */
  
  /* Set window */
  Display_Manager_SetWindow(x, y, x + size - 1, y + size - 1);
  
  /* Fill with color */
  Display_Manager_WriteCommand(GC9A01_RAMWR);
  
  for (uint32_t i = 0; i < size * size; i++) {
    Display_Manager_WriteData((color >> 8) & 0xFF);  /* High byte */
    Display_Manager_WriteData(color & 0xFF);  /* Low byte */
  }
}

/**
  * @brief  Initialize GC9A01 display controller
  * @param  None
  * @retval None
  */
static void Display_Manager_InitGC9A01(void)
{
  /* Reset display */
  HAL_GPIO_WritePin(DISPLAY_RST_PORT, DISPLAY_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(DISPLAY_RST_PORT, DISPLAY_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(120);
  
  /* Initialize display */
  Display_Manager_WriteCommand(GC9A01_SWRESET);  /* Software reset */
  HAL_Delay(120);
  
  Display_Manager_WriteCommand(GC9A01_SLPOUT);  /* Sleep out */
  HAL_Delay(120);
  
  Display_Manager_WriteCommand(GC9A01_MADCTL);  /* Memory data access control */
  Display_Manager_WriteData(0x08);  /* RGB order */
  
  Display_Manager_WriteCommand(GC9A01_COLMOD);  /* Interface pixel format */
  Display_Manager_WriteData(0x05);  /* 16-bit color */
  
  Display_Manager_WriteCommand(GC9A01_INVON);  /* Display inversion on */
  
  Display_Manager_WriteCommand(GC9A01_DISPON);  /* Display on */
  HAL_Delay(20);
  
  /* Turn on backlight */
  HAL_GPIO_WritePin(DISPLAY_BL_PORT, DISPLAY_BL_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Write command to GC9A01
  * @param  cmd: Command byte
  * @retval None
  */
static void Display_Manager_WriteCommand(uint8_t cmd)
{
  /* Set DC pin low for command */
  HAL_GPIO_WritePin(DISPLAY_DC_PORT, DISPLAY_DC_PIN, GPIO_PIN_RESET);
  
  /* Set CS pin low to select display */
  HAL_GPIO_WritePin(DISPLAY_CS_PORT, DISPLAY_CS_PIN, GPIO_PIN_RESET);
  
  /* Send command */
  HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
  
  /* Set CS pin high to deselect display */
  HAL_GPIO_WritePin(DISPLAY_CS_PORT, DISPLAY_CS_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Write data to GC9A01
  * @param  data: Data byte
  * @retval None
  */
static void Display_Manager_WriteData(uint8_t data)
{
  /* Set DC pin high for data */
  HAL_GPIO_WritePin(DISPLAY_DC_PORT, DISPLAY_DC_PIN, GPIO_PIN_SET);
  
  /* Set CS pin low to select display */
  HAL_GPIO_WritePin(DISPLAY_CS_PORT, DISPLAY_CS_PIN, GPIO_PIN_RESET);
  
  /* Send data */
  HAL_SPI_Transmit(&hspi1, &data, 1, 10);
  
  /* Set CS pin high to deselect display */
  HAL_GPIO_WritePin(DISPLAY_CS_PORT, DISPLAY_CS_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Set display window for drawing
  * @param  x0: Start X coordinate
  * @param  y0: Start Y coordinate
  * @param  x1: End X coordinate
  * @param  y1: End Y coordinate
  * @retval None
  */
static void Display_Manager_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  /* Set column address */
  Display_Manager_WriteCommand(GC9A01_CASET);
  Display_Manager_WriteData(x0 >> 8);
  Display_Manager_WriteData(x0 & 0xFF);
  Display_Manager_WriteData(x1 >> 8);
  Display_Manager_WriteData(x1 & 0xFF);
  
  /* Set row address */
  Display_Manager_WriteCommand(GC9A01_RASET);
  Display_Manager_WriteData(y0 >> 8);
  Display_Manager_WriteData(y0 & 0xFF);
  Display_Manager_WriteData(y1 >> 8);
  Display_Manager_WriteData(y1 & 0xFF);
}

/**
  * @brief  Update a display item
  * @param  item: Pointer to display item structure
  * @retval None
  */
static void Display_Manager_UpdateItem(Display_Item_t* item)
{
  if (item == NULL || !item->enabled) {
    return;
  }
  
  /* Get value for the item */
  uint32_t value = Display_Manager_GetItemValue(item);
  
  /* Draw item based on its type */
  switch (item->type) {
    case DISPLAY_ITEM_TEXT:
      /* Draw label */
      Display_Manager_DrawText(item->x, item->y, item->label, item->color);
      break;
    
    case DISPLAY_ITEM_VALUE:
      /* Draw label */
      Display_Manager_DrawText(item->x, item->y, item->label, item->color);
      
      /* Draw value */
      Display_Manager_DrawValue(item->x + strlen(item->label) * 8 + 5, item->y, value, item->color);
      break;
    
    case DISPLAY_ITEM_BAR:
      /* Draw label */
      Display_Manager_DrawText(item->x, item->y, item->label, item->color);
      
      /* Draw bar */
      Display_Manager_DrawBar(item->x, item->y + 20, item->width, item->height - 20, value, item->color);
      break;
    
    case DISPLAY_ITEM_ICON:
      /* Draw icon */
      Display_Manager_DrawIcon(item->x, item->y, value, item->color);
      break;
    
    default:
      break;
  }
}

/**
  * @brief  Get value for a display item
  * @param  item: Pointer to display item structure
  * @retval uint32_t: Value for the item
  */
static uint32_t Display_Manager_GetItemValue(Display_Item_t* item)
{
  uint32_t value = 0;
  
  /* Get value based on data source */
  switch (item->dataSource) {
    case DISPLAY_DATA_SOURCE_SERIAL:
      /* In a real implementation, this would get data from serial buffer */
      value = 0;
      break;
    
    case DISPLAY_DATA_SOURCE_CAN:
      /* In a real implementation, this would get data from CAN buffer */
      value = 0;
      break;
    
    case DISPLAY_DATA_SOURCE_SYSTEM:
      /* Get system parameter */
      switch (item->source.system.paramId) {
        case 0:  /* System status */
          value = 1;  /* 1 = OK */
          break;
        
        case 1:  /* Connected devices */
          value = 2;  /* 2 devices */
          break;
        
        case 2:  /* Active mappings */
          value = 5;  /* 5 mappings */
          break;
        
        case 3:  /* System uptime */
          value = HAL_GetTick() / 1000;  /* Seconds */
          break;
        
        default:
          value = 0;
          break;
      }
      break;
    
    default:
      value = 0;
      break;
  }
  
  return value;
}
