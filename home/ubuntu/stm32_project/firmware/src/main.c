/**
 * @file main.c
 * @brief Main application entry point for STM32F407 HID to Serial/CAN project
 * @author Manus AI
 * @date March 2025
 * 
 * This file contains the main application entry point and initialization routines
 * for the STM32F407 HID to Serial/CAN project. It initializes all hardware peripherals,
 * sets up the USB host for HID devices, configures serial and CAN outputs, and
 * initializes the display.
 * 
 * The main loop handles the processing of input events from HID devices and
 * maps them to the appropriate outputs based on the current configuration.
 * 
 * References:
 * - STM32F407 Reference Manual: https://www.st.com/resource/en/reference_manual/dm00031020.pdf
 * - STM32F4 HAL Documentation: https://www.st.com/resource/en/user_manual/dm00105879.pdf
 * - USB HID Specification: https://www.usb.org/document-library/device-class-definition-hid-111
 */

#include "main.h"
#include "usb_host.h"
#include "input_manager.h"
#include "mapping_engine.h"
#include "output_manager.h"
#include "display_manager.h"
#include "web_server.h"
#include "tunerstudio.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Private function prototypes */
static void SystemClock_Config(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);
static void InitDebugSerial(void);
static void PrintDebugInfo(void);

/* Global variables */
UART_HandleTypeDef huart2; /* UART handle for debug output */
volatile uint32_t systemTicks = 0; /* System tick counter */
uint8_t debugBuffer[256]; /* Buffer for debug messages */

/**
 * @brief  The application entry point.
 * @retval int - Return value (not used in embedded applications)
 */
int main(void)
{
  /* Configure the MPU for better memory protection */
  MPU_Config();
  
  /* Enable the CPU cache for better performance */
  CPU_CACHE_Enable();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick */
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();

  /* Initialize debug serial port for USB debugging output */
  InitDebugSerial();
  
  /* Print initial debug information */
  PrintDebugInfo();

  /* Initialize all configured peripherals */
  DEBUG_PRINT("Initializing GPIO...\r\n");
  MX_GPIO_Init();
  
  DEBUG_PRINT("Initializing USB Host...\r\n");
  MX_USB_HOST_Init();
  
  DEBUG_PRINT("Initializing CAN...\r\n");
  MX_CAN_Init();
  
  DEBUG_PRINT("Initializing UART...\r\n");
  MX_UART_Init();
  
  DEBUG_PRINT("Initializing SPI for display...\r\n");
  MX_SPI_Init();
  
  DEBUG_PRINT("Initializing Ethernet for web server...\r\n");
  MX_ETH_Init();

  /* Initialize application modules */
  DEBUG_PRINT("Initializing Input Manager...\r\n");
  InputManager_Init();
  
  DEBUG_PRINT("Initializing Mapping Engine...\r\n");
  MappingEngine_Init();
  
  DEBUG_PRINT("Initializing Output Manager...\r\n");
  OutputManager_Init();
  
  DEBUG_PRINT("Initializing Display Manager...\r\n");
  DisplayManager_Init();
  
  DEBUG_PRINT("Initializing Web Server...\r\n");
  WebServer_Init();
  
  DEBUG_PRINT("Initializing TunerStudio Interface...\r\n");
  TunerStudio_Init();

  DEBUG_PRINT("System initialization complete. Entering main loop.\r\n");
  
  /* Display welcome message on the TFT */
  DisplayManager_ShowWelcomeScreen();

  /* Main application loop */
  while (1)
  {
    /* Process USB Host events (HID device connections, data) */
    MX_USB_HOST_Process();
    
    /* Process input events from connected HID devices */
    InputEvent_t inputEvent;
    if (InputManager_GetNextEvent(&inputEvent))
    {
      DEBUG_PRINT("Input event received: Type=%d, Device=%d, Code=%d, Value=%d\r\n", 
                 inputEvent.type, inputEvent.deviceId, inputEvent.code, inputEvent.value);
      
      /* Map input event to output actions using the mapping engine */
      OutputAction_t outputActions[MAX_ACTIONS_PER_EVENT];
      uint8_t numActions = 0;
      
      MappingEngine_MapEvent(&inputEvent, outputActions, &numActions);
      
      /* Execute all mapped output actions */
      for (uint8_t i = 0; i < numActions; i++)
      {
        DEBUG_PRINT("Executing output action: Type=%d, Target=%d, Value=%d\r\n", 
                   outputActions[i].type, outputActions[i].target, outputActions[i].value);
        OutputManager_ExecuteAction(&outputActions[i]);
      }
    }
    
    /* Update display with latest information */
    DisplayManager_Update();
    
    /* Process web server requests */
    WebServer_Process();
    
    /* Process TunerStudio communication */
    TunerStudio_Process();
    
    /* Toggle heartbeat LED to indicate system is running */
    if ((HAL_GetTick() % 500) == 0)
    {
      HAL_GPIO_TogglePin(HEARTBEAT_LED_GPIO_Port, HEARTBEAT_LED_Pin);
    }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  * 
  * This function configures the system clock to run at 168 MHz using the
  * external high-speed oscillator (HSE) and the PLL.
  * 
  * Reference: STM32F407 Reference Manual, Section 7 (Reset and Clock Control)
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Configure the main internal regulator output voltage */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Initialize the CPU, AHB and APB buses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  
  DEBUG_PRINT("System clock configured at 168 MHz\r\n");
}

/**
  * @brief  Initialize the debug serial port (UART2) for USB debugging output
  * @retval None
  * 
  * This function initializes UART2 for debug output over USB.
  * Baud rate: 115200
  * Word length: 8 bits
  * Stop bits: 1
  * Parity: None
  * Flow control: None
  */
static void InitDebugSerial(void)
{
  /* Configure UART2 pins */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* Enable GPIO clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_USART2_CLK_ENABLE();
  
  /* Configure TX and RX pins */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure UART */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Send initial message to confirm UART is working */
  HAL_UART_Transmit(&huart2, (uint8_t*)"Debug UART initialized\r\n", 24, 100);
}

/**
  * @brief  Print system debug information
  * @retval None
  * 
  * This function prints system information for debugging purposes,
  * including firmware version, clock configuration, and memory usage.
  */
static void PrintDebugInfo(void)
{
  DEBUG_PRINT("\r\n\r\n");
  DEBUG_PRINT("=================================================\r\n");
  DEBUG_PRINT("  STM32F407 HID to Serial/CAN Project\r\n");
  DEBUG_PRINT("  Firmware Version: 1.0.0\r\n");
  DEBUG_PRINT("  Build Date: " __DATE__ " " __TIME__ "\r\n");
  DEBUG_PRINT("=================================================\r\n");
  DEBUG_PRINT("  CPU: STM32F407VGT6 - ARM Cortex-M4 @ 168MHz\r\n");
  DEBUG_PRINT("  Flash: 1MB, RAM: 192KB\r\n");
  DEBUG_PRINT("  System Clock: %lu MHz\r\n", HAL_RCC_GetSysClockFreq() / 1000000);
  DEBUG_PRINT("  HCLK: %lu MHz\r\n", HAL_RCC_GetHCLKFreq() / 1000000);
  DEBUG_PRINT("  APB1: %lu MHz\r\n", HAL_RCC_GetPCLK1Freq() / 1000000);
  DEBUG_PRINT("  APB2: %lu MHz\r\n", HAL_RCC_GetPCLK2Freq() / 1000000);
  DEBUG_PRINT("=================================================\r\n");
  DEBUG_PRINT("  Starting initialization...\r\n\r\n");
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  * 
  * This function handles critical errors by:
  * 1. Printing debug information about the error
  * 2. Blinking the error LED rapidly
  * 3. Optionally resetting the system after a timeout
  */
static void Error_Handler(void)
{
  DEBUG_PRINT("ERROR: Critical system error occurred!\r\n");
  
  /* Infinite error handling loop */
  while (1)
  {
    /* Blink error LED rapidly to indicate system error */
    HAL_GPIO_TogglePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin);
    HAL_Delay(100);
  }
}

/**
  * @brief  Configure the MPU (Memory Protection Unit)
  * @retval None
  * 
  * This function configures the MPU to protect critical memory regions
  * and improve system stability.
  * 
  * Reference: ARM Cortex-M4 Generic User Guide, Section 8.4 (MPU)
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes for SRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x20000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  CPU L1-Cache enable.
  * @retval None
  * 
  * This function enables the CPU cache for better performance.
  * The STM32F407 has separate instruction and data caches.
  * 
  * Reference: ARM Cortex-M4 Technical Reference Manual, Section 4.3 (Cache)
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  Debug print function that outputs to the debug UART
  * @param  format: Formatted string (printf style)
  * @param  ...: Variable arguments
  * @retval None
  * 
  * This function formats and sends debug messages to the UART.
  * It is used throughout the code for debugging purposes.
  */
void DEBUG_PRINT(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  
  /* Format the string */
  vsnprintf((char*)debugBuffer, sizeof(debugBuffer), format, args);
  
  /* Send over UART */
  HAL_UART_Transmit(&huart2, debugBuffer, strlen((char*)debugBuffer), 100);
  
  va_end(args);
}

/**
  * @brief  This function handles System tick timer.
  * @retval None
  * 
  * This function is called by the HAL_IncTick() function which is called
  * by the SysTick handler. It increments the system tick counter.
  */
void HAL_IncTick(void)
{
  systemTicks++;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  DEBUG_PRINT("ASSERT FAILED: file %s on line %lu\r\n", file, line);
  
  /* Infinite loop */
  while (1)
  {
    HAL_GPIO_TogglePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin);
    HAL_Delay(100);
  }
}
#endif /* USE_FULL_ASSERT */
