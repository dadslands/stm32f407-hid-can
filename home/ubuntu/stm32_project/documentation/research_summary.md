# Research Summary for STM32F407 HID to Serial/CAN Project

## STM32F407 Specifications
- ARM Cortex-M4 32-bit MCU with FPU
- Up to 168 MHz CPU frequency
- 210 DMIPS performance
- Up to 1MB Flash memory
- 192+4KB RAM
- USB OTG HS/FS
- Ethernet
- 17 Timers
- 3 ADCs
- 15 communication interfaces
- CAN bus support (2 × CAN interfaces)
- Multiple GPIO pins for interfacing

## GC9A01 Display Controller (1.28" 240x240 Round TFT)
- Resolution: 240 × 240 pixels
- Display size: Φ32.4mm
- Interface: 4-wire SPI
- Operating voltage: 3.3V / 5V
- Display Panel: IPS
- Pixel size: 0.135 × 0.135mm
- Dimensions: 40.4 × 37.5mm Φ37.5mm

### GC9A01 Pin Configuration
- VCC: Power (3.3V / 5V input)
- GND: Ground
- DIN: SPI data input
- CLK: SPI clock input
- CS: Chip selection, low active
- DC: Data/Command selection (high for data, low for command)
- RST: Reset, low active
- BL: Backlight

## HID Device Handling on STM32
- STM32F407 supports USB Host functionality
- USB HID class can be implemented using STM32 USB Host library
- HID devices (keyboards, mice, joysticks, etc.) can be connected and their inputs read
- Multiple HID devices can be supported through USB hub
- Input data can be processed and mapped to outputs

## CAN Bus Implementation on STM32
- STM32F407 has built-in CAN controllers
- Supports standard and extended frame formats
- Configurable bit rates
- Filter capabilities for message acceptance
- Interrupt-driven or polling-based operation
- Multiple CAN interfaces available (2x)

## TunerStudio Integration
- TunerStudio communicates with ECUs using a specific protocol
- Integration requires creating an .ini file that defines the data structure
- Communication typically happens over serial interface
- Custom firmware needs to implement the protocol expected by TunerStudio
- Data mapping between HID inputs and CAN/serial outputs can be configured through TunerStudio

## Web Interface Requirements
- Static web page for configuration and firmware updates
- Needs to provide interface for:
  - Firmware uploading and flashing
  - Input-to-output mapping configuration
  - Display configuration
  - TunerStudio integration settings
- Can be hosted on the STM32F407 itself using embedded web server
