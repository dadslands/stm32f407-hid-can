# STM32F407 HID to Serial/CAN Project - Technical Documentation

## Firmware Architecture

### Overview

The firmware architecture follows a layered approach to ensure modularity, maintainability, and extensibility. The system is designed to handle multiple HID input devices, map their inputs to either serial or CAN bus outputs, and provide configuration through both a web interface and TunerStudio.

### Layer Structure

#### Hardware Abstraction Layer (HAL)
- Provides low-level access to hardware peripherals
- Abstracts hardware-specific details from higher layers
- Includes drivers for STM32F407 peripherals, USB Host, SPI, UART, CAN, etc.

#### Middleware Layer
- Provides higher-level functionality built on top of the HAL
- Includes USB Host stack, file system, web server, etc.
- Handles protocol implementations (HID, CAN, TunerStudio)

#### Application Layer
- Implements the core functionality of the system
- Includes input processing, mapping engine, output management, etc.
- Handles user interface and configuration

### Core Components

#### USB Host Stack
- Manages USB device enumeration and communication
- Handles HID device detection and report parsing
- Supports multiple simultaneous HID devices

```c
// USB Host initialization
void USB_Host_Init(void) {
    // Initialize USB Host HAL
    USB_Host_HAL_Init();
    
    // Register HID class driver
    USB_Host_RegisterClass(&HID_Class_Driver);
    
    // Start USB Host
    USB_Host_Start();
}

// HID device connection callback
void USB_Host_HID_DeviceConnected(USB_HID_DeviceInfo_t* deviceInfo) {
    // Add device to connected devices list
    Input_Manager_AddDevice(deviceInfo);
    
    // Start receiving reports
    USB_Host_HID_StartReportReception(deviceInfo->deviceID);
}
```

#### Input Manager
- Processes input data from HID devices
- Normalizes inputs from different device types
- Provides a unified interface for the mapping engine

```c
// Input event structure
typedef struct {
    uint8_t deviceID;
    uint8_t inputType;
    uint16_t inputCode;
    int32_t value;
} Input_Event_t;

// Process HID report
void Input_Manager_ProcessReport(uint8_t deviceID, uint8_t* report, uint16_t reportLength) {
    // Parse report based on device type
    Input_Event_t events[MAX_EVENTS_PER_REPORT];
    uint8_t eventCount = 0;
    
    // Extract events from report
    HID_Parser_ExtractEvents(deviceID, report, reportLength, events, &eventCount);
    
    // Process each event
    for (uint8_t i = 0; i < eventCount; i++) {
        Mapping_Engine_ProcessInput(&events[i]);
    }
}
```

#### Mapping Engine
- Maps input events to output actions
- Applies transformations and conditions
- Manages mapping configurations

```c
// Mapping structure
typedef struct {
    uint8_t sourceDeviceID;
    uint8_t sourceInputType;
    uint16_t sourceInputCode;
    uint8_t destinationType;
    uint8_t destinationID;
    uint16_t destinationCode;
    int32_t minInputValue;
    int32_t maxInputValue;
    int32_t minOutputValue;
    int32_t maxOutputValue;
    bool invert;
    uint8_t conditionType;
    uint32_t conditionValue;
} Mapping_t;

// Process input event
void Mapping_Engine_ProcessInput(Input_Event_t* event) {
    // Find mappings for this input
    Mapping_t* mappings[MAX_MAPPINGS_PER_INPUT];
    uint8_t mappingCount = 0;
    
    Mapping_Engine_FindMappings(event, mappings, &mappingCount);
    
    // Process each mapping
    for (uint8_t i = 0; i < mappingCount; i++) {
        // Check conditions
        if (Mapping_Engine_CheckCondition(mappings[i], event)) {
            // Apply transformation
            int32_t outputValue = Mapping_Engine_TransformValue(mappings[i], event->value);
            
            // Send to output
            Output_Manager_SendOutput(mappings[i]->destinationType, 
                                     mappings[i]->destinationID,
                                     mappings[i]->destinationCode,
                                     outputValue);
        }
    }
}
```

#### Output Manager
- Handles output to serial and CAN bus
- Formats data according to configuration
- Manages output queuing and timing

```c
// Output types
#define OUTPUT_TYPE_SERIAL 0
#define OUTPUT_TYPE_CAN    1

// Send output
void Output_Manager_SendOutput(uint8_t outputType, uint8_t outputID, uint16_t outputCode, int32_t value) {
    switch (outputType) {
        case OUTPUT_TYPE_SERIAL:
            Output_Manager_SendSerial(outputID, outputCode, value);
            break;
            
        case OUTPUT_TYPE_CAN:
            Output_Manager_SendCAN(outputID, outputCode, value);
            break;
    }
}

// Send serial output
void Output_Manager_SendSerial(uint8_t portID, uint16_t dataID, int32_t value) {
    // Format data according to configuration
    uint8_t buffer[16];
    uint8_t length = 0;
    
    Serial_FormatData(portID, dataID, value, buffer, &length);
    
    // Send data
    Serial_Send(portID, buffer, length);
}

// Send CAN output
void Output_Manager_SendCAN(uint8_t busID, uint16_t messageID, int32_t value) {
    // Format CAN message
    CAN_Message_t message;
    
    CAN_FormatMessage(busID, messageID, value, &message);
    
    // Send message
    CAN_Send(busID, &message);
}
```

#### Display Manager
- Controls the GC9A01 display
- Manages UI elements and layouts
- Handles display updates and animations

```c
// Display item types
#define DISPLAY_ITEM_TEXT  0
#define DISPLAY_ITEM_VALUE 1
#define DISPLAY_ITEM_BAR   2
#define DISPLAY_ITEM_ICON  3

// Display item structure
typedef struct {
    uint8_t type;
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
    uint16_t color;
    char* label;
    uint8_t dataSource;
    uint16_t dataID;
    uint32_t refreshRate;
} Display_Item_t;

// Update display
void Display_Manager_Update(void) {
    static uint32_t lastUpdateTime = 0;
    uint32_t currentTime = HAL_GetTick();
    
    // Update each item based on refresh rate
    for (uint8_t i = 0; i < displayItemCount; i++) {
        if (currentTime - lastUpdateTime >= displayItems[i].refreshRate) {
            Display_Manager_UpdateItem(&displayItems[i]);
        }
    }
    
    lastUpdateTime = currentTime;
}
```

#### Web Server
- Serves the configuration web interface
- Handles HTTP requests and responses
- Processes configuration updates

```c
// Web server initialization
void Web_Server_Init(void) {
    // Initialize Ethernet
    Ethernet_Init();
    
    // Initialize HTTP server
    HTTP_Server_Init();
    
    // Register URI handlers
    HTTP_Server_RegisterHandler("/", Web_Server_HandleRoot);
    HTTP_Server_RegisterHandler("/config", Web_Server_HandleConfig);
    HTTP_Server_RegisterHandler("/update", Web_Server_HandleUpdate);
    HTTP_Server_RegisterHandler("/api/mappings", Web_Server_HandleMappingsAPI);
    HTTP_Server_RegisterHandler("/api/devices", Web_Server_HandleDevicesAPI);
    
    // Start server
    HTTP_Server_Start();
}

// Handle configuration API
void Web_Server_HandleMappingsAPI(HTTP_Request_t* request, HTTP_Response_t* response) {
    if (strcmp(request->method, "GET") == 0) {
        // Get mappings
        char* mappingsJSON = Mapping_Engine_GetMappingsJSON();
        
        // Set response
        HTTP_Response_SetContent(response, mappingsJSON, strlen(mappingsJSON));
        HTTP_Response_SetContentType(response, "application/json");
        
        free(mappingsJSON);
    }
    else if (strcmp(request->method, "POST") == 0) {
        // Update mappings
        Mapping_Engine_UpdateMappingsFromJSON(request->body);
        
        // Set response
        HTTP_Response_SetStatus(response, 200);
        HTTP_Response_SetContent(response, "{\"status\":\"ok\"}", 15);
        HTTP_Response_SetContentType(response, "application/json");
    }
}
```

#### TunerStudio Integration
- Implements the TunerStudio protocol
- Provides access to configuration parameters
- Enables real-time data monitoring

```c
// TunerStudio command types
#define TS_CMD_ECHO            0x01
#define TS_CMD_READ_MEMORY     0x02
#define TS_CMD_WRITE_MEMORY    0x03
#define TS_CMD_READ_BLOCK      0x04
#define TS_CMD_BURN_BLOCK      0x05
#define TS_CMD_READ_PAGE       0x06
#define TS_CMD_BURN_PAGE       0x07

// Process TunerStudio command
void TunerStudio_ProcessCommand(uint8_t* data, uint16_t length) {
    uint8_t command = data[0];
    
    switch (command) {
        case TS_CMD_ECHO:
            TunerStudio_HandleEcho(data, length);
            break;
            
        case TS_CMD_READ_MEMORY:
            TunerStudio_HandleReadMemory(data, length);
            break;
            
        case TS_CMD_WRITE_MEMORY:
            TunerStudio_HandleWriteMemory(data, length);
            break;
            
        // Handle other commands...
    }
}
```

### Memory Management

The firmware uses a combination of static and dynamic memory allocation:

- **Static Allocation**: Used for fixed-size structures and buffers
- **Dynamic Allocation**: Used for variable-size data like configuration

Memory is organized into several regions:

- **Flash**: Stores program code and default configuration
- **RAM**: Used for runtime data and temporary buffers
- **EEPROM Emulation**: Stores user configuration

### Task Scheduling

The firmware uses a cooperative multitasking approach:

```c
// Main loop
void main(void) {
    // Initialize system
    System_Init();
    
    // Main loop
    while (1) {
        // Process USB events
        USB_Host_Process();
        
        // Update display
        Display_Manager_Update();
        
        // Process web server
        Web_Server_Process();
        
        // Process TunerStudio
        TunerStudio_Process();
        
        // Process system tasks
        System_ProcessTasks();
    }
}
```

### Error Handling

The firmware implements a comprehensive error handling system:

```c
// Error codes
typedef enum {
    ERROR_NONE = 0,
    ERROR_USB_HOST_INIT_FAILED,
    ERROR_DISPLAY_INIT_FAILED,
    ERROR_ETHERNET_INIT_FAILED,
    ERROR_CAN_INIT_FAILED,
    ERROR_SERIAL_INIT_FAILED,
    ERROR_CONFIG_LOAD_FAILED,
    // More error codes...
} Error_Code_t;

// Error handler
void Error_Handler(Error_Code_t errorCode) {
    // Log error
    System_LogError(errorCode);
    
    // Display error
    Display_Manager_ShowError(errorCode);
    
    // Take appropriate action based on error
    switch (errorCode) {
        case ERROR_USB_HOST_INIT_FAILED:
            // Try to reinitialize USB Host
            USB_Host_DeInit();
            USB_Host_Init();
            break;
            
        // Handle other errors...
        
        default:
            // For critical errors, reset system
            if (IS_CRITICAL_ERROR(errorCode)) {
                System_Reset();
            }
            break;
    }
}
```

## Web Interface Design

### Architecture

The web interface is designed as a single-page application (SPA) with a RESTful API backend:

- **Frontend**: HTML, CSS, and JavaScript
- **Backend**: Embedded web server on the STM32F407
- **Communication**: JSON-based API

### Frontend Structure

```
web/
├── index.html          # Main HTML file
├── css/
│   ├── main.css        # Main stylesheet
│   └── bootstrap.min.css # Bootstrap framework
├── js/
│   ├── main.js         # Main JavaScript file
│   ├── mappings.js     # Mapping configuration
│   ├── display.js      # Display configuration
│   ├── firmware.js     # Firmware update
│   └── api.js          # API communication
└── img/
    └── logo.png        # Logo and other images
```

### API Endpoints

The web interface communicates with the firmware through these API endpoints:

- **GET /api/status**: Get system status
- **GET /api/devices**: Get connected devices
- **GET /api/mappings**: Get current mappings
- **POST /api/mappings**: Update mappings
- **GET /api/display**: Get display configuration
- **POST /api/display**: Update display configuration
- **POST /api/firmware**: Upload firmware update

### User Interface

The web interface is organized into several sections:

1. **Dashboard**: Overview of system status
2. **Devices**: List of connected HID devices
3. **Mappings**: Configuration of input-to-output mappings
4. **Display**: Configuration of the TFT display
5. **Settings**: System settings
6. **Firmware**: Firmware update

### Responsive Design

The interface is designed to work on various screen sizes:

- **Desktop**: Full layout with all features
- **Tablet**: Optimized layout with slightly reduced features
- **Mobile**: Simplified layout focusing on essential features

### Security

The web interface implements basic security measures:

- **Authentication**: Optional password protection
- **Input Validation**: All user inputs are validated
- **CSRF Protection**: Tokens used to prevent cross-site request forgery

## Build System

### Makefile

The build system uses a Makefile to compile the firmware:

```makefile
# Project name
PROJECT = stm32f407_hid_to_can

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# MCU flags
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Compiler flags
CFLAGS = $(MCU) -Wall -Wextra -O2 -g
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -I$(INC_DIR)
CFLAGS += -DSTM32F407xx -DUSE_HAL_DRIVER

# Linker flags
LDFLAGS = $(MCU) -specs=nano.specs -T stm32f407vg_flash.ld
LDFLAGS += -Wl,--gc-sections

# Libraries
LIBS = -lc -lm -lnosys

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.s)

# Object files
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.s=$(OBJ_DIR)/%.o)

# Include STM32 HAL
CFLAGS += -I$(PWD)/lib/STM32CubeF4/STM32F4xx_HAL_Driver/Inc
CFLAGS += -I$(PWD)/lib/STM32CubeF4/CMSIS/Device/ST/STM32F4xx/Include
CFLAGS += -I$(PWD)/lib/STM32CubeF4/CMSIS/Include

# Add HAL source files
HAL_SRC = $(wildcard lib/STM32CubeF4/STM32F4xx_HAL_Driver/Src/*.c)
OBJ_FILES += $(HAL_SRC:lib/%.c=$(OBJ_DIR)/%.o)

# Targets
.PHONY: all clean flash

all: $(BIN_DIR)/$(PROJECT).bin $(BIN_DIR)/$(PROJECT).hex

$(BIN_DIR)/$(PROJECT).bin: $(BIN_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

$(BIN_DIR)/$(PROJECT).hex: $(BIN_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $< $@

$(BIN_DIR)/$(PROJECT).elf: $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)
	$(SIZE) $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s | $(OBJ_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/lib/%.o: lib/%.c | $(OBJ_DIR)/lib
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN_DIR) $(OBJ_DIR) $(OBJ_DIR)/lib:
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

flash: $(BIN_DIR)/$(PROJECT).bin
	st-flash write $(BIN_DIR)/$(PROJECT).bin 0x8000000
```

### Dependencies

The firmware depends on several libraries:

- **STM32CubeF4**: HAL and CMSIS for STM32F407
- **FatFS**: File system for configuration storage
- **lwIP**: TCP/IP stack for Ethernet communication

### Build Process

The build process consists of these steps:

1. Compile C and assembly files
2. Link object files
3. Generate binary and hex files
4. Calculate size and memory usage

### Flashing

The firmware can be flashed using ST-Link:

```bash
st-flash write bin/stm32f407_hid_to_can.bin 0x8000000
```

## Testing Strategy

### Unit Testing

Unit tests are implemented for critical components:

- **Mapping Engine**: Test input-to-output mapping logic
- **HID Parser**: Test HID report parsing
- **Configuration**: Test configuration loading/saving

### Integration Testing

Integration tests verify the interaction between components:

- **USB Host + Input Manager**: Test HID device handling
- **Mapping Engine + Output Manager**: Test end-to-end mapping
- **Web Server + Configuration**: Test configuration updates

### System Testing

System tests verify the complete syst<response clipped><NOTE>To save on context only part of this file has been shown to you. You should retry this tool after you have searched inside the file with `grep -n` in order to find the line numbers of what you are looking for.</NOTE>