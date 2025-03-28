/**
 * @file web_server.h
 * @brief Web server interface for configuration and firmware updates
 * @author Manus AI
 * @date March 2025
 * 
 * This file contains the declarations for the web server functionality
 * that provides a web interface for device configuration, input-to-output
 * mapping, and firmware updates.
 * 
 * References:
 * - lwIP documentation: https://www.nongnu.org/lwip/2_1_x/index.html
 * - HTTP/1.1 specification: https://tools.ietf.org/html/rfc2616
 * - STM32F4 Ethernet documentation: https://www.st.com/resource/en/application_note/dm00053141.pdf
 */

#ifndef __WEB_SERVER_H
#define __WEB_SERVER_H

#include "main.h"
#include <stdint.h>

/**
 * @brief Initialize the web server
 * 
 * This function initializes the web server, setting up the network stack
 * and HTTP server. It prepares the server to handle configuration and
 * firmware update requests.
 * 
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef WebServer_Init(void);

/**
 * @brief Process web server requests
 * 
 * This function processes incoming HTTP requests. It should be called
 * regularly in the main loop to handle web server operations.
 * 
 * @return None
 */
void WebServer_Process(void);

/**
 * @brief Get the web server status
 * 
 * This function returns the current status of the web server,
 * including connection count and request count.
 * 
 * @param connectionCount Pointer to store the connection count
 * @param requestCount Pointer to store the request count
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef WebServer_GetStatus(uint32_t *connectionCount, uint32_t *requestCount);

/**
 * @brief Reset the web server
 * 
 * This function resets the web server, clearing all connections
 * and reinitializing the network stack. It can be used to recover
 * from error states or to force reconnection after firmware updates.
 * 
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef WebServer_Reset(void);

/**
 * @brief Debug function to print web server status
 * 
 * This function prints detailed information about the web server
 * status, including connection count, request count, and active connections.
 * 
 * @return None
 */
void WebServer_DebugPrint(void);

#endif /* __WEB_SERVER_H */
