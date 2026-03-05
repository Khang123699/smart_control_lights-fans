# Embedded Smart Home System

![Language](https://img.shields.io/badge/language-C-orange.svg)
![Platform](https://img.shields.io/badge/platform-STM32-lightgrey.svg)

An automated smart home system that dynamically controls lighting and ventilation based on real-time sensors (temperature, light, presence), while also allowing manual control via touch buttons and providing OLED monitoring. The project is designed with a finite state machine architecture to process multiple sensor streams concurrently, ensuring a stable, energy-efficient, and highly responsive living environment.

## Features

- **Environmental Automation**: Automatically controls fans and lights based on real-time room temperature (DHT11) and ambient light levels (LDR).
- **Human Presence Detection**: Uses a high-precision mmWave Radar (HLK-LD2410C) to detect both moving and static human presence, saving energy when rooms are empty.
- **Dual Control Modes**: Seamlessly switch between **AUTO** (sensor-driven) and **MANUAL** (manual control via touch buttons) modes.
- **Real-Time Monitoring**: Visualizes current temperature, humidity, light levels, and device statuses on an OLED SSD1306 display.
- **Remote PC Interface**: Supports Serial Communication (UART) for PC-based monitoring and remote control.

## Hardware Requirements

- **Microcontroller**: STM32 Series (e.g., STM32F103 / STM32F4)
- **Sensors**: 
  - DHT11 (Temperature & Humidity)
  - Photoresistor / LDR (Ambient Light via ADC)
  - HLK-LD2410C mmWave Radar (Human Presence via UART)
- **Actuators**: 3-Channel Relay Module (Active Low) for 2 LEDs (Garden & Indoor) and 1 Ventilation Fan.
- **UI Components**: 
  - TTP223 Touch Buttons (Manual Control)
  - 0.96" OLED Display I2C (SSD1306)

## Project Architecture (Modular Design)

The firmware is written in C using the Hardware Abstraction Layer (HAL) and is structured into modular components:

* `sensor_manager`: Handles data acquisition from DHT11 (1-Wire), LDR (ADC), and Radar (UART parsing).
* `device_control`: State-machine logic deciding when to trigger relays based on thresholds and user modes.
* `touch_button`: Scans touch inputs with robust debounce logic for mode switching or manual overrides.
* `user_interface`: Manages the OLED display to show real-time metrics.
* `pc_serial_com`: UART interface for sending telemetry data to a connected PC.

## Getting Started

### Prerequisites
- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (or Keil uVision with STM32CubeMX).
- ST-Link V2 Programmer.

### Installation
1. Clone this repository or download the source code.
2. Open the `.project` or `.ioc` file using STM32CubeIDE.
3. Build the project (`Project -> Build All`).
4. Connect your STM32 board via ST-Link and flash the firmware (`Run -> Debug`).

### Pin Configuration (Default)
| Component | STM32 Pin | Interface |
| :--- | :--- | :--- |
| **DHT11 Data** | `PA1` | GPIO + Timer1 (1-Wire) |
| **LDR Sensor** | `PA0` | ADC1 |
| **Radar TX/RX** | `USART1` | UART Interrupt |
| **Relay 1 (Fan)** | `PB0` | GPIO Output |
| **Relay 2 (LED1)** | `PB1` | GPIO Output |
| **Relay 3 (LED2)** | `PB10` | GPIO Output |
*(Note: Please refer to `main.c` or the STM32CubeMX `.ioc` file for the exact pinout mappings).*

## Usage

1. **Power On**: Upon powering the system, the OLED will display a "Welcome" screen and initialize sensors.
2. **Auto Mode**: By default, the system runs in AUTO mode. 
   - Garden lights turn on when it's dark.
   - Indoor lights turn on when it's dark AND someone is in the room.
   - The fan turns on when the temperature exceeds 30°C AND someone is present.
3. **Manual Mode**: Touch the capacitive buttons to toggle lights/fans manually, overriding the sensor logic.
4. **PC Monitoring**: Connect the MCU to a PC via a USB-to-TTL adapter (baud rate: 115200) to view real-time data logs.
