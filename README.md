# KZZCV2.0 Project Documentation
CN: [[README_CN.md]] EN: [[README.md]]
## Project Overview
KZZCV2.0 is an embedded GUI project based on the LVGL graphics library and FreeRTOS real-time operating system. The project adopts a modular design, supports multiple hardware platforms, and provides rich user interfaces and functionalities.

## Directory Structure
```
src/
├── core/                 # Core functionality module
│   ├── main.c           # Main program entry
│   └── tasks.h          # Task definitions and configurations
├── ui/                   # User interface module
│   ├── main/            # Main interface
│   ├── settings/        # Settings interface
│   ├── uart/            # UART interface
│   ├── chart/           # Chart interface
│   ├── usb/             # USB interface
│   └── common/          # Common UI components
├── drivers/             # Hardware driver module
│   ├── lcd/             # LCD driver
│   ├── touch/           # Touch driver
│   └── adc/             # ADC driver
├── middleware/          # Middleware module
│   ├── lvgl/            # LVGL middleware
│   └── usb/             # USB middleware
└── utils/               # Utility module
    └── malloc/          # Memory management
```

## Module Description

### 1. Core Module (core)
- **main.c**: Main program entry, responsible for system initialization and task creation
  - System clock configuration
  - Peripheral initialization
  - FreeRTOS task creation
  - System scheduler startup
- **tasks.h**: Task definitions and configurations
  - Task priority definitions
  - Task stack size configurations
  - Task handle declarations
  - Queue handle declarations

### 2. User Interface Module (ui)
- **main_ui**: Main interface module
  - Main interface layout
  - Status bar display
  - Navigation buttons
  - Time and date display
- **settings_ui**: Settings interface module
  - System information display
  - Hardware information display
  - Software information display
  - About information display
- **uart_ui**: UART interface module
  - UART parameter configuration
  - Data transmission display
  - Data format settings
  - Baud rate settings
- **chart_ui**: Chart interface module
  - Temperature curve display
  - Bar chart display
  - Data scaling control
  - Data clear function
- **usb_ui**: USB interface module
  - USB connection status display
  - USB data transmission
  - USB device management

### 3. Driver Module (drivers)
- **lcd**: LCD driver module
  - LCD initialization
  - Display buffer management
  - Display refresh control
  - Display area settings
- **touch**: Touch driver module
  - Touch initialization
  - Touch data reading
  - Touch event handling
  - Touch coordinate conversion
- **adc**: ADC driver module
  - ADC initialization
  - Data acquisition control
  - Temperature calculation
  - Data conversion

### 4. Middleware Module (middleware)
- **lvgl**: LVGL middleware
  - LVGL initialization
  - Display device management
  - Input device management
  - Timer management
- **usb**: USB middleware
  - USB device initialization
  - Data transmission management
  - Device status monitoring
  - Interrupt handling

### 5. Utility Module (utils)
- **malloc**: Memory management module
  - Memory pool management
  - Memory allocation
  - Memory deallocation
  - Memory usage statistics

## Framework Description

### 1. LVGL Graphics Library
- Version requirement: LVGL 8.x
- Features:
  - Rich UI components
  - Animation support
  - Theme customization
  - Multi-language support
  - Touch input support
  - Keyboard input support

### 2. FreeRTOS Real-Time Operating System
- Version requirement: FreeRTOS 10.x
- Features:
  - Task management
  - Memory management
  - Queue management
  - Semaphore management
  - Timer management
  - Interrupt management

## Hardware Requirements

### 1. Recommended MCU Models
- STM32F4 Series
  - STM32F407ZGT6
  - STM32F429ZIT6
  - STM32F469ZIT6
- STM32H7 Series
  - STM32H743ZIT6
  - STM32H750ZBT6

### 2. Performance Requirements
- CPU frequency: ≥168MHz
- RAM: ≥128KB
- Flash: ≥512KB
- External RAM: ≥8MB (optional)

### 3. Peripheral Requirements
- LCD interface: RGB/8080/SPI
- Touch interface: I2C/SPI
- USB interface: USB 2.0 Full Speed
- ADC: 12-bit resolution
- UART: At least 2

### 4. Display Requirements
- Resolution: ≥480x320
- Color depth: ≥16-bit
- Refresh rate: ≥60Hz
- Touch: Multi-touch support

## Compilation and Running

### 1. Development Environment
- IDE: Keil MDK 5.x
- Compiler: ARMCC 5.x
- Debugger: ST-Link V2

### 2. Compilation Steps
1. Open project file
2. Configure target MCU model
3. Configure compilation options
4. Compile project
5. Download program

### 3. Running Requirements
- Ensure correct hardware connections
- Ensure stable power supply
- Ensure debugger connection is normal
- Ensure LCD and touch screen are working properly

## Notes
1. System clock configuration required for first use
2. Correct peripheral pin configuration required
3. Correct interrupt priority configuration required
4. Correct FreeRTOS parameter configuration required
5. Correct LVGL parameter configuration required

## Maintenance and Updates
1. Regular code update checks
2. Regular dependency library updates
3. Regular code optimization
4. Regular performance testing
5. Regular stability testing