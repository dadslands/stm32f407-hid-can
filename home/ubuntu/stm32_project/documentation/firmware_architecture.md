# STM32F407 HID to Serial/CAN Firmware Architecture

## System Overview

This firmware architecture enables the STM32F407 to serve as a bridge between multiple HID input devices and serial/CAN outputs. The system includes a web interface for configuration and firmware updates, as well as TunerStudio integration for advanced configuration.

## Core Components

### 1. Hardware Abstraction Layer (HAL)
- **STM32 HAL**: Provides low-level drivers for hardware peripherals
- **Board Support Package (BSP)**: Specific implementations for STM32mega dev board
- **Peripheral Drivers**: USB Host, CAN, SPI, UART, GPIO, etc.

### 2. Middleware Layer
- **USB Host Stack**: Manages USB host functionality and device enumeration
- **HID Class Driver**: Handles HID device communication and input parsing
- **File System**: Manages configuration storage and firmware update files
- **Web Server**: Lightweight HTTP server for configuration interface
- **Display Driver**: Controls the GC9A01 display controller

### 3. Application Layer
- **Input Manager**: Processes HID device inputs
- **Mapping Engine**: Maps inputs to outputs based on configuration
- **Output Manager**: Handles serial and CAN output generation
- **Configuration Manager**: Stores and retrieves system configuration
- **Firmware Update Manager**: Handles firmware update process
- **TunerStudio Interface**: Implements TunerStudio protocol for integration
- **Display UI Manager**: Controls display content and UI

## Module Descriptions

### Input Manager
- Detects and enumerates connected HID devices
- Reads input data from multiple HID devices
- Normalizes input data for consistent processing
- Provides input events to the Mapping Engine

### Mapping Engine
- Loads mapping configuration from storage
- Maps input events to output actions based on configuration
- Supports complex mappings (combinations, sequences, etc.)
- Provides mapped actions to Output Manager

### Output Manager
- Manages serial port communication
- Manages CAN bus communication
- Formats and sends data according to configured protocols
- Handles output timing and prioritization

### Configuration Manager
- Stores configuration in non-volatile memory
- Provides configuration interface to other modules
- Handles configuration validation and defaults
- Supports configuration import/export

### Web Interface
- Provides HTTP server for configuration access
- Serves static web pages for configuration UI
- Handles API requests for configuration changes
- Supports firmware update through web interface

### TunerStudio Interface
- Implements TunerStudio communication protocol
- Provides data exchange with TunerStudio
- Handles configuration through TunerStudio
- Generates and uses TunerStudio INI files

### Display UI Manager
- Controls the GC9A01 display
- Renders UI elements and data visualizations
- Shows system status and active mappings
- Provides visual feedback for user actions

## Data Flow

1. **Input Processing**:
   - HID devices → USB Host → HID Class Driver → Input Manager → Mapping Engine

2. **Output Generation**:
   - Mapping Engine → Output Manager → Serial/CAN Drivers → Physical Outputs

3. **Configuration**:
   - Web Interface/TunerStudio → Configuration Manager → All Modules

4. **Display Updates**:
   - System Events → Display UI Manager → GC9A01 Driver → Physical Display

## Memory Management

- **Flash Memory Allocation**:
  - Bootloader: 32KB
  - Application: 768KB
  - Configuration Storage: 128KB
  - Web Interface Files: 96KB

- **RAM Allocation**:
  - System and HAL: 32KB
  - USB Host Stack: 32KB
  - Application Logic: 64KB
  - Web Server: 32KB
  - Display Buffer: 32KB

## Task Prioritization

1. **High Priority**:
   - USB Host processing
   - CAN bus communication
   - Critical system events

2. **Medium Priority**:
   - Input mapping
   - Serial communication
   - TunerStudio communication

3. **Low Priority**:
   - Web server
   - Display updates
   - Configuration storage

## Firmware Update Process

1. User uploads new firmware through web interface
2. System validates firmware integrity
3. System stores firmware in temporary storage
4. System reboots into bootloader mode
5. Bootloader flashes new firmware
6. System reboots with new firmware
7. System verifies successful update

## Error Handling

- Comprehensive error detection and reporting
- Fallback to safe defaults on configuration errors
- Watchdog timer for system stability
- Error logging for diagnostics
- Visual error indication on display

## Future Expansion

- Support for additional input device types
- Enhanced mapping capabilities
- Additional output protocols
- Expanded display functionality
- Advanced configuration options
