# Flappy Bird on PIC18F8722

An embedded Flappy Bird game developed with two teammates for **CENG336 - Introduction to Embedded Systems Development** at METU. The game runs on a **PIC18F8722 at 40 MHz**, using four rows of LEDs as the playfield and a three-digit seven-segment display for the score.

Our work covers the game logic, interrupt handling, timing, and score display. The project builds on the starter project, device configuration, and map supplied for the assignment.

## Features

- Interrupt-driven flap and scroll-speed controls.
- A scrolling **4 x 8 LED playfield** with a repeating obstacle map.
- Bird blinking every **50 ms** and gravity every **750 ms**.
- Adjustable scroll interval from **250 to 1500 ms**, starting at **1000 ms**.
- Score tracking for passed pipe columns and survival time.
- A four-second freeze after a collision, followed by the one-second initialization period before the next round.

## Hardware and Controls

| Connection | Purpose |
|---|---|
| PORTB0 / INT0 | Flap upward one row |
| PORTB1 / INT1 | Increase scroll interval by 250 ms (slower) |
| PORTB3 / INT3 | Decrease scroll interval by 250 ms (faster) |
| PORTF, PORTE, PORTD, PORTC | LED rows, from top to bottom |
| PORTJ[0:6] | Seven-segment outputs a-g |
| PORTH[1:3] | Hundreds, tens, and units digit selection |

The assignment originally uses **PORTB2** for the faster-scroll input. This implementation uses **PORTB3** instead, following the hardware issue announcement documented in the original submission. The button interrupts use rising edges.

## Gameplay

The bird starts at **PORTD0**. Flapping moves it up one row; gravity moves it down one row. The bird stays within the top and bottom rows. Obstacles enter at bit 7 and move toward bit 0.

- Passing an occupied pipe column earns **2 points**.
- Surviving each **2 seconds** earns **3 points**.
- The score is capped at **999**.
- Hitting a pipe freezes the game and score for four seconds. The LEDs then clear and the score resets during the one-second initialization period.

The repeating map is defined in `the2.X/map.h`.

## Build and Run

1. Install **MPLAB X IDE** and **XC8** with PIC18F8722 device support. The included project is configured for **MPLAB X 6.20 / XC8 3.10**; the assignment originally specifies **MPLAB X 5.45 / XC8 2.30**.
2. Open `the2.X` through **File > Open Project** and select your installed compiler in Project Properties.
3. Choose **Simulate** for MPLAB Simulator, or **UploadWithPickit3** for the board with a PICkit 3 programmer.
4. Select **Clean and Build Project**. For hardware play, select **Make and Program Device** with the board connected.

In MPLAB Simulator, configure the clock for **40 MHz** and provide rising-edge inputs on RB0, RB1, and RB3. Playing on hardware requires the matching LED/button and seven-segment board connections shown above.

## Project Structure

- `the2.X/` - MPLAB project, C sources, headers, and build configuration.
- `CENG336_THE2.pdf` - Original Flappy Bird assignment specification.
- `the2.X/docs/` - Older assignment material retained from the starter project; the root PDF describes this game.
