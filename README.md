# computer-architecture-cw
A hardware-driven, bare-metal implementation of the classic Ping-Pong game written in C for an ARM-based microcontroller. The game reads real-time player input from analog dials (potentiometers) and renders graphics directly onto an oscilloscope screen by manipulating internal registers and steering the cathode-ray beam via DAC outputs over SPI.

# Bare-Metal Oscilloscope Ping-Pong (C / ARM)

A hardware-driven, bare-metal implementation of the classic Ping-Pong game written in C for an ARM-based microcontroller. The game reads real-time player input from analog dials (potentiometers) and renders graphics directly onto an oscilloscope screen by manipulating internal registers and steering the cathode-ray beam via DAC outputs over SPI.

## 🚀 Features
* **Bare-Metal Control:** No operating system or high-level display libraries. Fully configured using direct register manipulation (PMC, PIO, SPI, ADC).
* **Vector Graphics Display:** Generates a real-time game board, paddles, and movement by converting Cartesian coordinates into analog steering signals for an oscilloscope.
* **Dual Analog Controls:** Samples hardware potentiometers via ADC channels 4 and 5 to dynamically track paddle positions.
* **Polished Codebase:** Fixed legacy syntax bugs and added software input-clamping to prevent peripheral crashes during intense gameplay.

## 🛠️ Hardware & Protocols Used
* **Microcontroller:** ARM-based development board
* **Display:** Analogue Oscilloscope (XY Mode)
* **Peripherals Handles:** * **SPI:** Configured as Master to transmit coordinate packets to the DAC.
  * **ADC:** Continuous sampling of hardware paddles with software boundary interpolation.
  * **PIO / PMC:** Power management and peripheral pin routing.

## 📂 Repository Structure
* `flash3.c` - Core game engine, peripheral initialization, and rendering routines.
* `report.pdf` - Original academic laboratory report detailing the engineering design decisions.

## 🕹️ Graphics Pipeline (How it works)
The display treats the oscilloscope screen as a 1024x1024 grid. Points are rendered by packing the target X or Y coordinates with the designated peripheral port offset, shifting bits to account for the DAC floor, and pushing the payload through the Serial Peripheral Interface (SPI) Transmit Data Register.
