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

---

## Report

### 1 Introduction

The aim of this project was to develop a video game of ping pong for two players, in which they each control the vertical positions of the paddles to prevent the ball from hitting the edge of the border behind each of the paddle. An oscilloscope is used to generate the display of the image, and each bat is positioned using a hand held variable resistor "paddle". The two player game was also required to change the relative speeds of the ball as it bounced back from each paddle towards the opponent, and the score of each player is to be recorded ideally, either with a digital scoreboard until the game announces a winner, or for each score to be displayed on the Oscilloscope display.

### 2 Design

For the execution of this game, it required the use of three main pieces of hardware: the ARM processor, an Oscilloscope for output display, and the two paddles from which the players interacted with the game. Key components of the design included the border and other visual elements on the screen that required a digital to analogue converter built into the ARM board connected through the Serial Peripheral Interface (SPI), and this required to be reset and enabled; the PIO controller also needed to be configured as part of the SPI function. The paddles required reading the analogue inputs from the ARM processor by communicating with the ADC control registers to reset and enable channels 4 and 5 used to control the paddles (to move in a vertical fashion within its interpolated ranges). To set the levels on the analogue outputs to draw coordinates onto the oscilloscope, the SPI registers were manipulated and a point was output using Ports A and B. These two analogue signals are used to steer the oscilloscope beam in the x and y directions.

After enabling the correct peripherals and registers from the ARM processor for displaying on the oscilloscope and reading the analogue inputs, we concluded that it was better to reset the registers within a separate method (`resetRegisters()`) so that when each game was reset, the method can be called to reset each of the registers, effectively resetting the game. Upon reflection, the score should have been recorded for each successive game before re-compiling — this design fails to do so, however, with the time available this was the most efficient method devised; it also promoted more readable code. Following suit, all blocks of code were defined within methods called from the main function. The main design before implementation included two bats, a border, and a circular ball whose speed increased upon impact with the bats but decreased with the distance it travelled.

### 3 Implementation

Among other methods during the implementation, we created a function to check for serialisation — this involves checking if the SPI-TDR line has been transmitted to the serialisers ready for conversion to an analogue signal. This forces the code to wait for the SPI conversion from digital to analogue signal before displaying on the oscilloscope, ensuring it does not interfere with the serialisation processor.

During implementation of outputting to the oscilloscope, we experimented with different ways of outputting to the screen, and found the most efficient way was to treat the screen as a Cartesian plane of x–y coordinates. We therefore created a method `point(int x, int y)` that takes two values representing a point on the plane to ease drawing shapes. This method required setting the SPI-TDR registers to the A–B ports, then adding the chosen coordinate value. We concluded that the maximum value in either direction extended to 1023, which was defined as `maxLength`/`maxWidth` to increase readability and serve as the endpoint of the for loops used when drawing lines.

These lines required the two lowest bits (D0 and D1) to be set to 0 when using DAC outputs; therefore, when implementing the methods, coordinates had to be shifted left by two — `(x << 2)` and `(y << 2)` — before adding to the separate ports (`0x4000` and `0xc000`). This ensured that the points were drawn relative to the coordinates inputted to each method. The vertical and horizontal line methods were used to create straight lines, separated into the different elements of the game: border, paddles, and ball.

The next step was to implement the paddle function using channels 4 and 5. This enabled values to be read from the paddles by initiating conversion using the ARM chip's ADC control register. The challenge was figuring out how to constrain the paddles, which required interpolation to find the maximum and minimum values to determine how far the bats should move on the display. The status register had to be checked to wait for the channel to complete conversion before the value could be stored within the `ADC_CDR4` and `ADC_CDR5` registers.

The main design required a condition for losing — defined as when the ball's border coordinates matched the game border coordinates, causing the opposing player to score a point. Both this design point and changing the ball speed upon bat contact were omitted from the final code due to time constraints.

### 4 Results

By the end of the fourth week, the oscilloscope displayed all the main visual elements of the ping pong game, including a border, moveable paddles, and a ball in the centre of the screen as the game resets.

### 5 Reflections

Initially, we attempted to draw the border by drawing the top, bottom, and side lines in order. However, we realised that the code was far more readable when we made a function that created a square using horizontal and vertical lines. The advantage of creating a `square()` function was that it could be reused to make the paddles, which is more efficient than writing two separate pieces of code.

Originally, we tried to create a function for a circle to represent the ball but were not successful in the labs, so the square function was reused for the ball. In hindsight, the failure was due to a trivial syntax error; upon reflection, code making use of the equation `x² + y² = r²` for drawing circles on the Cartesian plane would have been more appropriate, and this implementation is present in the codebase within the `ball()` function.

There was also the issue of finding the breakpoints within the code to determine the maximum and minimum values of the paddles. Whenever the paddles came into contact with the border, the program crashed and the display failed. The fix required putting constraints on the values the paddles can take in the vertical direction so they did not collide with the border, while also ensuring the gap between each paddle and the border was not large enough for the ball to escape. With more time, this would have been fully implemented.
