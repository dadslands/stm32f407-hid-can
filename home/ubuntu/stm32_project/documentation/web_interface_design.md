# Web Interface Design for STM32F407 HID to Serial/CAN Project

## Overview

The web interface will provide a user-friendly way to configure the STM32F407 device, map HID inputs to serial/CAN outputs, update firmware, and integrate with TunerStudio. The interface will be served directly from the STM32F407 device.

## Key Features

### 1. Device Status Dashboard
- Connected HID devices display
- Current mapping configuration summary
- System status and diagnostics
- Display configuration preview

### 2. Input-to-Output Mapping Configuration
- Visual mapping interface
- HID device selection and input identification
- Output type selection (Serial or CAN)
- Output parameter configuration
- Mapping rules and conditions
- Save/load mapping profiles

### 3. Firmware Update
- Current firmware version display
- Firmware upload interface
- Update progress indication
- Update history log
- Rollback capability

### 4. TunerStudio Integration
- TunerStudio INI file generation
- TunerStudio connection settings
- Protocol configuration
- Data mapping for TunerStudio

### 5. Display Configuration
- Display content selection
- Layout customization
- Data source selection (CAN, Serial, System)
- Refresh rate and update settings
- Visual theme selection

### 6. System Settings
- Network configuration
- Security settings
- Logging options
- Backup and restore
- Factory reset

## Technical Implementation

### Web Server
- Lightweight HTTP server running on STM32F407
- Static file serving for HTML, CSS, and JavaScript
- RESTful API for dynamic data and configuration
- WebSocket for real-time updates

### Frontend
- Responsive design for desktop and mobile access
- Modern JavaScript framework (lightweight)
- SVG-based visualization for mapping
- Local storage for temporary data

### Backend
- JSON-based configuration storage
- Binary firmware update handling
- Authentication and authorization
- Configuration validation

### API Endpoints
- `/api/status` - System status information
- `/api/devices` - Connected HID devices
- `/api/mapping` - Input-to-output mapping configuration
- `/api/firmware` - Firmware update functionality
- `/api/tunerstudio` - TunerStudio integration settings
- `/api/display` - Display configuration
- `/api/settings` - System settings

## User Interface Mockup

### Main Dashboard
```
+-----------------------------------------------+
| STM32F407 HID to Serial/CAN Configuration     |
+---------------+-------------------------------+
| Navigation    | System Status                 |
|               |                               |
| • Dashboard   | Connected Devices:            |
| • Mapping     | • Keyboard (VID:PID)          |
| • Display     | • Joystick (VID:PID)          |
| • TunerStudio |                               |
| • Firmware    | Active Mappings: 12           |
| • Settings    |                               |
|               | System Uptime: 3d 4h 12m      |
|               |                               |
+---------------+-------------------------------+
| Current Display Preview                       |
|                                               |
| [Display Content Visualization]               |
|                                               |
+-----------------------------------------------+
```

### Mapping Configuration
```
+-----------------------------------------------+
| Input-to-Output Mapping                       |
+-----------------------------------------------+
| Input Device: [Dropdown] ▼                    |
| Input Action: [Dropdown] ▼                    |
|                                               |
| Output Type: ○ Serial  ● CAN                  |
|                                               |
| CAN ID: [0x123]                               |
| CAN Data: [Byte Configuration]                |
|                                               |
| Conditions: [Optional Trigger Conditions]     |
|                                               |
| [Add Mapping] [Save Profile] [Load Profile]   |
+-----------------------------------------------+
| Current Mappings                              |
|                                               |
| [Table of Configured Mappings]                |
| [Edit] [Delete] [Duplicate]                   |
|                                               |
+-----------------------------------------------+
```

### Firmware Update
```
+-----------------------------------------------+
| Firmware Update                               |
+-----------------------------------------------+
| Current Version: v1.0.0                       |
| Build Date: 2025-03-28                        |
|                                               |
| [Select Firmware File] [Upload and Install]   |
|                                               |
| Update History:                               |
| • v1.0.0 (2025-03-28) - Initial release       |
| • v0.9.0 (2025-03-15) - Beta version          |
|                                               |
| [Rollback to Previous Version]                |
+-----------------------------------------------+
```

## Responsive Design

The interface will adapt to different screen sizes:
- Desktop: Full layout with side navigation
- Tablet: Condensed layout with dropdown navigation
- Mobile: Stacked layout with hamburger menu

## Security Considerations

- Local network access only by default
- Optional password protection
- HTTPS support (if hardware resources allow)
- Input validation for all user inputs
- Rate limiting for API requests

## Performance Optimization

- Minified static assets
- Efficient JSON data structures
- Incremental updates via WebSocket
- Lazy loading of UI components
- Caching of static resources
