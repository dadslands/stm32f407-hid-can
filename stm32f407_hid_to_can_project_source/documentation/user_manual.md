# STM32F407 HID to Serial/CAN Project Documentation

## Project Overview

This project implements a versatile input-to-output mapping system for the STM32F407 microcontroller, specifically designed for the STM32mega development board. The system allows multiple HID input devices (keyboards, mice, gamepads, etc.) to be connected to the STM32F407, with their inputs mapped to either serial or CAN bus outputs.

### Key Features

1. **Multiple HID Device Support**: Connect and use various USB HID devices simultaneously
2. **Flexible Mapping System**: Map any input to any output with configurable parameters
3. **Dual Output Options**: Send mapped inputs to Serial UART or CAN bus
4. **1.28" Round TFT Display**: Visual feedback and status information via GC9A01 controller
5. **Web Interface**: Configure mappings and update firmware through a browser
6. **TunerStudio Integration**: Alternative configuration through TunerStudio software
7. **Firmware Updates**: Easy firmware updates through the web interface

## Hardware Requirements

- STM32F407 microcontroller (STM32mega development board)
- 1.28" Round TFT display with GC9A01 controller
- USB Host capability (for HID device connection)
- CAN transceiver (for CAN bus communication)
- Ethernet connection (for web interface access)

## Firmware Architecture

The firmware is designed with a modular architecture to ensure maintainability and extensibility:

### Hardware Abstraction Layer (HAL)
- **STM32F4xx HAL**: Base hardware abstraction for the microcontroller
- **USB Host HAL**: Interface for USB host functionality
- **Display HAL**: Interface for the GC9A01 display controller
- **Serial HAL**: Interface for UART communication
- **CAN HAL**: Interface for CAN bus communication

### Middleware Layer
- **USB Host Stack**: Manages USB device enumeration and communication
- **HID Parser**: Interprets HID reports from various devices
- **Web Server**: Serves the configuration web interface
- **File System**: Manages configuration storage
- **TunerStudio Protocol**: Handles communication with TunerStudio

### Application Layer
- **Input Manager**: Processes inputs from HID devices
- **Mapping Engine**: Maps inputs to outputs based on configuration
- **Output Manager**: Sends mapped data to serial or CAN bus
- **Display Manager**: Controls the TFT display
- **Configuration Manager**: Handles system configuration

## Input Device Handling

The system supports various HID devices including:

1. **Keyboards**: All standard keys and modifiers
2. **Mice**: X/Y movement, buttons, and scroll wheel
3. **Gamepads**: Buttons, analog sticks, and triggers
4. **Custom HID Devices**: Configurable through the mapping system

HID devices are automatically detected and enumerated when connected. The system can handle multiple devices simultaneously, with each device assigned a unique identifier for mapping purposes.

## Mapping System

The mapping engine allows for flexible configuration of how inputs are translated to outputs:

- **Source**: Any input from any connected HID device
- **Destination**: Serial UART or CAN bus
- **Transformation**: Optional scaling, inversion, or custom processing
- **Conditions**: Optional conditions for when mapping is active
- **Parameters**: Additional configuration for specific mappings

Mappings are stored in non-volatile memory and can be updated through the web interface or TunerStudio.

## Output System

### Serial Output
- Configurable baud rate (9600-921600)
- Customizable data format
- Optional checksums and framing

### CAN Bus Output
- Standard and extended IDs
- Configurable bit rate
- Customizable data format
- Support for multiple CAN messages

## Display System

The 1.28" round TFT display with GC9A01 controller provides visual feedback and status information:

- **Status Screen**: Shows connected devices and system status
- **Data Visualization**: Displays input and output data in real-time
- **Configuration**: Basic configuration options accessible directly
- **Alerts**: Visual notifications for important events

The display layout and content are configurable through the web interface.

## Web Interface

The web interface provides a user-friendly way to configure the system:

### Main Features
- **Dashboard**: Overview of system status and connected devices
- **Mapping Configuration**: Create, edit, and delete input-to-output mappings
- **Device Settings**: Configure system parameters
- **Display Configuration**: Customize the TFT display
- **Firmware Update**: Upload and apply firmware updates

The web interface is accessible via the device's IP address and works on any modern web browser.

## TunerStudio Integration

TunerStudio integration provides an alternative configuration method:

- **Custom INI File**: Defines the configuration parameters
- **Real-time Data**: View input and output data in real-time
- **Mapping Tables**: Configure input-to-output mappings
- **Settings**: Adjust system parameters

## Installation and Setup

1. Flash the firmware to the STM32F407 using ST-Link or similar programmer
2. Connect the GC9A01 display to the appropriate pins
3. Connect power and Ethernet
4. Access the web interface via the device's IP address
5. Configure as needed

## Pin Connections

### Display (GC9A01)
- CS: PA4
- DC: PA3
- RST: PA2
- BL: PA1
- SCK: PA5
- MISO: PA6
- MOSI: PA7

### USB Host
- USB_DM: PA11
- USB_DP: PA12
- USB_ID: PA10
- USB_VBUS: PA9

### CAN Bus
- CAN_TX: PB9
- CAN_RX: PB8

### Serial UART
- UART_TX: PA2
- UART_RX: PA3

## Troubleshooting

### Common Issues

1. **HID Device Not Detected**
   - Check USB connection
   - Ensure device is a supported HID class
   - Verify power supply is adequate

2. **Mapping Not Working**
   - Check mapping configuration
   - Verify input device is properly detected
   - Check output configuration

3. **Display Issues**
   - Verify display connections
   - Check SPI configuration
   - Ensure power supply is stable

4. **Web Interface Not Accessible**
   - Check Ethernet connection
   - Verify IP address
   - Ensure web server is running

5. **TunerStudio Connection Issues**
   - Check serial connection
   - Verify baud rate settings
   - Ensure correct INI file is loaded

## Future Enhancements

Planned future enhancements include:

1. Support for additional HID device types
2. Enhanced mapping capabilities with more transformation options
3. Additional output protocols
4. Expanded display functionality
5. Mobile app for configuration

## License

This project is released under the MIT License.

## Credits

Developed by Manus AI for custom STM32F407 HID to Serial/CAN mapping application.
