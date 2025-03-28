/**
 * @file output_manager.c
 * @brief Output Manager implementation for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date 2025-03-28
 */

/* Includes ------------------------------------------------------------------*/
#include "output_manager.h"
#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define OUTPUT_CONFIG_ADDR       0x08070000  /* Flash sector for configuration storage */
#define OUTPUT_CONFIG_SIZE       (sizeof(Serial_Config_t) + sizeof(CAN_Config_t) + 8)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
CAN_HandleTypeDef hcan1;

Serial_Config_t serialConfig;
CAN_Config_t canConfig;

uint8_t serialTxBuffer[MAX_SERIAL_BUFFER_SIZE];
uint8_t serialTxHead = 0;
uint8_t serialTxTail = 0;
uint8_t serialTxCount = 0;

uint8_t canTxBuffer[MAX_CAN_BUFFER_SIZE][8];
uint32_t canTxIds[MAX_CAN_BUFFER_SIZE];
uint8_t canTxLengths[MAX_CAN_BUFFER_SIZE];
uint8_t canTxHead = 0;
uint8_t canTxTail = 0;
uint8_t canTxCount = 0;

/* Private function prototypes -----------------------------------------------*/
static void Output_Manager_InitSerial(void);
static void Output_Manager_InitCAN(void);
static void Output_Manager_ProcessSerial(void);
static void Output_Manager_ProcessCAN(void);
static uint8_t Output_Manager_FormatSerialData(uint8_t* data, uint8_t length, uint8_t* formattedData);

/* External variables --------------------------------------------------------*/

/**
  * @brief  Output Manager initialization function
  * @param  None
  * @retval None
  */
void Output_Manager_Init(void)
{
  /* Load configuration from flash */
  Output_Manager_LoadConfig();
  
  /* Initialize serial interface */
  Output_Manager_InitSerial();
  
  /* Initialize CAN interface */
  Output_Manager_InitCAN();
  
  /* Initialize buffers */
  serialTxHead = 0;
  serialTxTail = 0;
  serialTxCount = 0;
  
  canTxHead = 0;
  canTxTail = 0;
  canTxCount = 0;
}

/**
  * @brief  Output Manager process function, should be called periodically
  * @param  None
  * @retval None
  */
void Output_Manager_Process(void)
{
  /* Process serial output */
  Output_Manager_ProcessSerial();
  
  /* Process CAN output */
  Output_Manager_ProcessCAN();
}

/**
  * @brief  Send data to serial output
  * @param  data: Pointer to data buffer
  * @param  length: Length of data
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Output_Manager_SendSerial(uint8_t* data, uint8_t length)
{
  if (!serialConfig.enabled || data == NULL || length == 0) {
    return 0;
  }
  
  /* Check if there's enough space in the buffer */
  if (serialTxCount + length > MAX_SERIAL_BUFFER_SIZE) {
    return 0;
  }
  
  /* Format data based on configuration */
  uint8_t formattedData[MAX_SERIAL_BUFFER_SIZE];
  uint8_t formattedLength = Output_Manager_FormatSerialData(data, length, formattedData);
  
  /* Add data to buffer */
  for (uint8_t i = 0; i < formattedLength; i++) {
    serialTxBuffer[serialTxTail] = formattedData[i];
    serialTxTail = (serialTxTail + 1) % MAX_SERIAL_BUFFER_SIZE;
    serialTxCount++;
  }
  
  return 1;
}

/**
  * @brief  Send data to CAN output
  * @param  canId: CAN identifier
  * @param  data: Pointer to data buffer
  * @param  length: Length of data (max 8 bytes)
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Output_Manager_SendCAN(uint32_t canId, uint8_t* data, uint8_t length)
{
  if (!canConfig.enabled || data == NULL || length == 0 || length > 8) {
    return 0;
  }
  
  /* Check if there's space in the buffer */
  if (canTxCount >= MAX_CAN_BUFFER_SIZE) {
    return 0;
  }
  
  /* Add message to buffer */
  canTxIds[canTxTail] = canId;
  canTxLengths[canTxTail] = length;
  
  for (uint8_t i = 0; i < length; i++) {
    canTxBuffer[canTxTail][i] = data[i];
  }
  
  canTxTail = (canTxTail + 1) % MAX_CAN_BUFFER_SIZE;
  canTxCount++;
  
  return 1;
}

/**
  * @brief  Configure serial interface
  * @param  config: Pointer to serial configuration structure
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Output_Manager_ConfigureSerial(Serial_Config_t* config)
{
  if (config == NULL) {
    return 0;
  }
  
  /* Copy configuration */
  memcpy(&serialConfig, config, sizeof(Serial_Config_t));
  
  /* Re-initialize serial interface */
  Output_Manager_InitSerial();
  
  return 1;
}

/**
  * @brief  Configure CAN interface
  * @param  config: Pointer to CAN configuration structure
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Output_Manager_ConfigureCAN(CAN_Config_t* config)
{
  if (config == NULL) {
    return 0;
  }
  
  /* Copy configuration */
  memcpy(&canConfig, config, sizeof(CAN_Config_t));
  
  /* Re-initialize CAN interface */
  Output_Manager_InitCAN();
  
  return 1;
}

/**
  * @brief  Get serial configuration
  * @param  None
  * @retval Serial_Config_t*: Pointer to serial configuration structure
  */
Serial_Config_t* Output_Manager_GetSerialConfig(void)
{
  return &serialConfig;
}

/**
  * @brief  Get CAN configuration
  * @param  None
  * @retval CAN_Config_t*: Pointer to CAN configuration structure
  */
CAN_Config_t* Output_Manager_GetCANConfig(void)
{
  return &canConfig;
}

/**
  * @brief  Save output configuration to flash
  * @param  None
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Output_Manager_SaveConfig(void)
{
  /* In a real implementation, this would save the configuration to flash memory
     using the STM32 HAL flash functions. For this example, we'll just return success. */
  
  return 1;
}

/**
  * @brief  Load output configuration from flash
  * @param  None
  * @retval uint8_t: 1 if successful, 0 if failed
  */
uint8_t Output_Manager_LoadConfig(void)
{
  /* In a real implementation, this would load the configuration from flash memory
     using the STM32 HAL flash functions. For this example, we'll set default values. */
  
  /* Set default serial configuration */
  serialConfig.enabled = 1;
  serialConfig.baudRate = 115200;
  serialConfig.dataBits = 8;
  serialConfig.stopBits = 1;
  serialConfig.parity = 0;
  serialConfig.format = SERIAL_FORMAT_RAW;
  
  /* Set default CAN configuration */
  canConfig.enabled = 1;
  canConfig.bitRate = 500000;
  canConfig.mode = 0;  /* Normal mode */
  canConfig.sjw = 1;
  canConfig.bs1 = 6;
  canConfig.bs2 = 7;
  
  return 1;
}

/**
  * @brief  Reset output configuration to defaults
  * @param  None
  * @retval None
  */
void Output_Manager_ResetConfig(void)
{
  /* Set default serial configuration */
  serialConfig.enabled = 1;
  serialConfig.baudRate = 115200;
  serialConfig.dataBits = 8;
  serialConfig.stopBits = 1;
  serialConfig.parity = 0;
  serialConfig.format = SERIAL_FORMAT_RAW;
  
  /* Set default CAN configuration */
  canConfig.enabled = 1;
  canConfig.bitRate = 500000;
  canConfig.mode = 0;  /* Normal mode */
  canConfig.sjw = 1;
  canConfig.bs1 = 6;
  canConfig.bs2 = 7;
  
  /* Re-initialize interfaces */
  Output_Manager_InitSerial();
  Output_Manager_InitCAN();
}

/**
  * @brief  Initialize serial interface
  * @param  None
  * @retval None
  */
static void Output_Manager_InitSerial(void)
{
  /* Configure UART peripheral */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = serialConfig.baudRate;
  
  /* Set data bits */
  if (serialConfig.dataBits == 9) {
    huart1.Init.WordLength = UART_WORDLENGTH_9B;
  } else {
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
  }
  
  /* Set stop bits */
  if (serialConfig.stopBits == 2) {
    huart1.Init.StopBits = UART_STOPBITS_2;
  } else {
    huart1.Init.StopBits = UART_STOPBITS_1;
  }
  
  /* Set parity */
  if (serialConfig.parity == 1) {
    huart1.Init.Parity = UART_PARITY_ODD;
  } else if (serialConfig.parity == 2) {
    huart1.Init.Parity = UART_PARITY_EVEN;
  } else {
    huart1.Init.Parity = UART_PARITY_NONE;
  }
  
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  
  /* Initialize UART */
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    Error_Handler();
  }
}

/**
  * @brief  Initialize CAN interface
  * @param  None
  * @retval None
  */
static void Output_Manager_InitCAN(void)
{
  /* Configure CAN peripheral */
  hcan1.Instance = CAN1;
  
  /* Calculate prescaler for desired bit rate */
  uint32_t apb1Clock = HAL_RCC_GetPCLK1Freq();
  uint32_t prescaler = apb1Clock / (canConfig.bitRate * (1 + canConfig.bs1 + canConfig.bs2));
  
  hcan1.Init.Prescaler = prescaler;
  hcan1.Init.Mode = canConfig.mode == 0 ? CAN_MODE_NORMAL : CAN_MODE_LOOPBACK;
  hcan1.Init.SyncJumpWidth = canConfig.sjw;
  hcan1.Init.TimeSeg1 = canConfig.bs1;
  hcan1.Init.TimeSeg2 = canConfig.bs2;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  
  /* Initialize CAN */
  if (HAL_CAN_Init(&hcan1) != HAL_OK) {
    Error_Handler();
  }
  
  /* Start CAN */
  if (HAL_CAN_Start(&hcan1) != HAL_OK) {
    Error_Handler();
  }
}

/**
  * @brief  Process serial output
  * @param  None
  * @retval None
  */
static void Output_Manager_ProcessSerial(void)
{
  /* Check if there's data to send and UART is ready */
  if (serialTxCount > 0 && __HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE)) {
    /* Send one byte */
    HAL_UART_Transmit(&huart1, &serialTxBuffer[serialTxHead], 1, 10);
    
    /* Update buffer pointers */
    serialTxHead = (serialTxHead + 1) % MAX_SERIAL_BUFFER_SIZE;
    serialTxCount--;
  }
}

/**
  * @brief  Process CAN output
  * @param  None
  * @retval None
  */
static void Output_Manager_ProcessCAN(void)
{
  /* Check if there's data to send and CAN is ready */
  if (canTxCount > 0 && HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {
    /* Prepare CAN message */
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;
    
    /* Set up header */
    if (canTxIds[canTxHead] <= 0x7FF) {
      txHeader.IDE = CAN_ID_STD;
      txHeader.StdId = canTxIds[canTxHead];
    } else {
      txHeader.IDE = CAN_ID_EXT;
      txHeader.ExtId = canTxIds[canTxHead];
    }
    
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = canTxLengths[canTxHead];
    txHeader.TransmitGlobalTime = DISABLE;
    
    /* Send message */
    if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, canTxBuffer[canTxHead], &txMailbox) == HAL_OK) {
      /* Update buffer pointers */
      canTxHead = (canTxHead + 1) % MAX_CAN_BUFFER_SIZE;
      canTxCount--;
    }
  }
}

/**
  * @brief  Format serial data based on configuration
  * @param  data: Pointer to input data buffer
  * @param  length: Length of input data
  * @param  formattedData: Pointer to output formatted data buffer
  * @retval uint8_t: Length of formatted data
  */
static uint8_t Output_Manager_FormatSerialData(uint8_t* data, uint8_t length, uint8_t* formattedData)
{
  uint8_t formattedLength = 0;
  
  /* Format data based on configuration */
  switch (serialConfig.format) {
    case SERIAL_FORMAT_RAW:
      /* Raw format - copy data as is */
      memcpy(formattedData, data, length);
      formattedLength = length;
      break;
    
    case SERIAL_FORMAT_ASCII:
      /* ASCII format - convert each byte to ASCII characters */
      for (uint8_t i = 0; i < length; i++) {
        uint8_t value = data[i];
        
        /* Convert to ASCII characters (2 hex digits) */
        uint8_t high = (value >> 4) & 0x0F;
        uint8_t low = value & 0x0F;
        
        formattedData[formattedLength++] = high < 10 ? high + '0' : high - 10 + 'A';
        formattedData[formattedLength++] = low < 10 ? low + '0' : low - 10 + 'A';
        
        /* Add space between bytes */
        if (i < length - 1) {
          formattedData[formattedLength++] = ' ';
        }
      }
      break;
    
    case SERIAL_FORMAT_BINARY_LSB:
      /* Binary format (LSB first) - copy data as is */
      memcpy(formattedData, data, length);
      formattedLength = length;
      break;
    
    case SERIAL_FORMAT_BINARY_MSB:
      /* Binary format (MSB first) - reverse byte order for multi-byte values */
      if (length == 2) {
        formattedData[0] = data[1];
        formattedData[1] = data[0];
        formattedLength = 2;
      } else if (length == 4) {
        formattedData[0] = data[3];
        formattedData[1] = data[2];
        formattedData[2] = data[1];
        formattedData[3] = data[0];
        formattedLength = 4;
      } else {
        /* For other lengths, just copy data */
        memcpy(formattedData, data, length);
        formattedLength = length;
      }
      break;
    
    case SERIAL_FORMAT_HEX:
      /* Hex format - convert to hex string with 0x prefix */
      formattedData[0] = '0';
      formattedData[1] = 'x';
      formattedLength = 2;
      
      for (uint8_t i = 0; i < length; i++) {
        uint8_t value = data[i];
        
        /* Convert to hex characters */
        uint8_t high = (value >> 4) & 0x0F;
        uint8_t low = value & 0x0F;
        
        formattedData[formattedLength++] = high < 10 ? high + '0' : high - 10 + 'A';
        formattedData[formattedLength++] = low < 10 ? low + '0' : low - 10 + 'A';
      }
      
      /* Add newline */
      formattedData[formattedLength++] = '\r';
      formattedData[formattedLength++] = '\n';
      break;
    
    default:
      /* Default to raw format */
      memcpy(formattedData, data, length);
      formattedLength = length;
      break;
  }
  
  return formattedLength;
}
