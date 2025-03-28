# TunerStudio Configuration Guide for STM32F407 HID-CAN Project

This document provides instructions for setting up and using TunerStudio with the STM32F407 HID-CAN project.

## Installation

1. Download and install TunerStudio MS from [EFI Analytics](https://www.tunerstudio.com/index.php/downloads)
2. Download the STM32F407 HID-CAN TunerStudio package from our website
3. Extract the package to a convenient location

## Setting Up a New Project

1. Open TunerStudio MS
2. Click "File" > "New Project"
3. Enter a name for your project
4. Click "Browse" next to "Project Folder" and select where you want to save the project
5. Click "Next"
6. Select "Other / Browse..." from the firmware dropdown
7. Browse to the location where you extracted the TunerStudio package and select the `stm32f407_hid_can.ini` file
8. Click "Next"
9. Configure your connection settings:
   - Select the appropriate COM port for your STM32F407 device
   - Set Baud Rate to 115200
   - Leave other settings at their defaults
10. Click "Next"
11. Click "Finish" to create the project

## Connecting to Your Device

1. Ensure your STM32F407 device is connected to your computer
2. In TunerStudio, click the "Connect" button in the toolbar
3. If the connection is successful, you should see "Connected" in the status bar

## Using the Dashboard

1. Click "View" > "Dashboard" > "Main Dashboard"
2. The dashboard displays:
   - Input values from connected HID devices
   - Output values being sent to Serial/CAN
   - System metrics like CPU load and memory usage
3. You can customize the dashboard by right-clicking and selecting "Edit Dashboard"

## Configuring Input-to-Output Mappings

1. Click "Settings" > "Inputs" to open the Input Devices dialog
2. View connected devices and their current input values
3. Click "Settings" > "Outputs" to open the Output Configuration dialog
4. Use the mapping table to configure how inputs are mapped to outputs
5. Click "Burn to ECU" to save your configuration to the device

## Troubleshooting

If you have trouble connecting to your device:

1. Verify the device is powered on and connected to your computer
2. Check that you've selected the correct COM port
3. Ensure the baud rate is set to 115200
4. Try disconnecting and reconnecting the device
5. Restart TunerStudio

## Advanced Configuration

For advanced users, you can:

1. Create custom dashboards by clicking "File" > "New Dashboard"
2. Modify gauge configurations in "Settings" > "Gauge Cluster Settings"
3. Create custom gauge layouts for specific monitoring needs
4. Set up logging by clicking "Datalog" > "Start Logging"

## Support

If you need assistance with TunerStudio configuration, please visit our GitHub repository at https://github.com/dadslands/stm32f407-hid-can or contact us through our website.
