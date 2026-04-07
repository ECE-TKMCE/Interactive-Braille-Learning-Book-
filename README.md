# Interactive Braille Learning Aid

An interactive, multi-page Braille learning book designed to teach the alphabet and numbers through tactile inputs and AI-powered vocal feedback. Built using a dual-microcontroller architecture (Arduino Uno + ESP32), this system provides real-time pronunciation verification using Edge Impulse.

## Key Features
* **Tactile Sensor Grid:** A 3x3 touch sensor matrix coupled with page-detecting sensors to accurately map a student's finger position to the correct Braille character.
* **Dynamic Paging System:** Supports A-Z and 1-9. The hardware logic dynamically updates character mapping based on the active page (e.g., Sensor 1 on Page 1 = 'A', Sensor 1 on Page 2 = 'J').
* **Listen Mode:** Touching a letter triggers an onboard DFPlayer Mini to audibly pronounce the letter for immediate feedback.
* **Read Mode (AI Verification):** The student touches a letter and speaks into an INMP441 I2S microphone. An onboard ESP32 runs a custom Edge Impulse Machine Learning model to verify the pronunciation, playing a "Correct" or "Try Again" sound based on confidence scores.

## Hardware Architecture

### Microcontrollers
* **Arduino Uno:** Acts as the primary sensor hub. Manages the 3x3 touch sensor grid and the page-detection sensors.
* **ESP32:** Acts as the DSP and ML engine. Receives character data from the Arduino, manages the Edge Impulse voice recognition model, and controls audio peripherals.

### Peripherals & Power
* **Audio Input:** INMP441 I2S Microphone (connected to ESP32).
* **Audio Output:** DFPlayer Mini + Speaker (connected to ESP32).
* **Sensors:** Capacitive touch sensors / push buttons arranged in a 3x3 matrix.
* **Power System:** 7.4V LiPo battery stepped down via a 4.7V buck converter to safely power the logic and audio modules.

## System Logic: Book Layout

The book is organized into physical pages, with the Arduino tracking the current page to interpret the 3x3 grid:
* **Page 1:** A to I (9 letters)
* **Page 2:** J to R (9 letters)
* **Page 3:** S to Z & Number 1 (9 characters)
* **Page X:** Continues up to Number 9.

*Example:* If the page sensor detects Page 2, and the 1st grid sensor goes HIGH, the Arduino identifies the character as 'J' and signals the ESP32 accordingly.

## Getting Started

### Prerequisites
* [Arduino IDE](https://www.arduino.cc/en/software)
* ESP32 Board Support Package installed in the Arduino IDE.
* DFRobotDFPlayerMini Library.
* Custom Edge Impulse C++ Library (included in the `Edge_Impulse_Model` folder).

### Installation
1.  Clone this repository:
    ```bash
    git clone [https://github.com/YourUsername/Braille-Learning-Aid.git](https://github.com/YourUsername/Braille-Learning-Aid.git)
    ```
2.  Flash the Arduino Uno with the code in `/Arduino_Grid_Logic`.
3.  Install your exported Edge Impulse Arduino library as a `.zip` in the Arduino IDE.
4.  Flash the ESP32 with the code in `/ESP32_Audio_ML`.
5.  Format a MicroSD card (FAT32), load your MP3 files for the DFPlayer into a folder named `mp3`, and insert it into the module.


## Authors

- Shabas A (ECE, TKMCE)
- Naseef K (ECE, TKMCE)
- Muhammed Hadhy P (ECE, TKMCE)
- Muhammed Nibras N (ECE, TKMCE)
