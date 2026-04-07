#include <SoftwareSerial.h>

// Set up SoftwareSerial to communicate with ESP32
// Arduino Pin 11 = RX (Connects to ESP32 TX)
// Arduino Pin 12 = TX (Connects to ESP32 RX via voltage divider)
SoftwareSerial espSerial(11, 12); 

// --- Pin Definitions ---
// The 9 pins for the 3x3 touch sensor grid
const int gridPins[9] = {2, 3, 4, 5, 6, 7, 8, 9, 10};

// The pins used to detect which page is open
// Using Analog pins as digital inputs to save standard digital pins
const int pagePins[3] = {A0, A1, A2}; 

// --- State Tracking ---
// Keeps track of the previous state of each sensor to prevent spamming
bool lastGridState[9] = {false, false, false, false, false, false, false, false, false};

// Default to page 1
int currentPage = 1; 

void setup() {
  // Start hardware serial for PC debugging
  Serial.begin(9600);
  
  // Start software serial for ESP32 communication
  espSerial.begin(9600);
  Serial.println("Braille Book System: Arduino Initialized.");

  // Initialize grid pins as inputs
  for (int i = 0; i < 9; i++) {
    pinMode(gridPins[i], INPUT); 
  }

  // Initialize page pins as inputs
  for (int i = 0; i < 3; i++) {
    pinMode(pagePins[i], INPUT);
  }
}

void loop() {
  updateCurrentPage();
  scanTouchGrid();
  
  // Small delay to help with hardware debouncing
  delay(50); 
}

// Function to check which page is currently active
void updateCurrentPage() {
  if (digitalRead(pagePins[0]) == HIGH) {
    currentPage = 1;
  } else if (digitalRead(pagePins[1]) == HIGH) {
    currentPage = 2;
  } else if (digitalRead(pagePins[2]) == HIGH) {
    currentPage = 3;
  }
}

// Function to scan the 3x3 grid and detect touches
void scanTouchGrid() {
  for (int i = 0; i < 9; i++) {
    bool isTouched = digitalRead(gridPins[i]) == HIGH;

    // Trigger only on the "rising edge" (the exact moment it is touched)
    if (isTouched && !lastGridState[i]) {
      char characterDetected = determineCharacter(currentPage, i);
      
      if (characterDetected != '\0') {
        // Print to PC for debugging
        Serial.print("Page: ");
        Serial.print(currentPage);
        Serial.print(" | Sensor: ");
        Serial.print(i + 1);
        Serial.print(" | Character: ");
        Serial.println(characterDetected);

        // Send the character to the ESP32
        espSerial.print(characterDetected);
      }
    }
    
    // Update the state for the next loop
    lastGridState[i] = isTouched;
  }
}

// Function to map the page and sensor number to a specific character
char determineCharacter(int page, int sensorIndex) {
  // Page 1: A to I (9 letters)
  if (page == 1) {
    return 'A' + sensorIndex; 
  }
  
  // Page 2: J to R (9 letters)
  else if (page == 2) {
    return 'J' + sensorIndex;
  }
  
  // Page 3: S to Z (8 letters) + Number '1'
  else if (page == 3) {
    if (sensorIndex < 8) {
      return 'S' + sensorIndex; // S through Z
    } else if (sensorIndex == 8) {
      return '1'; // 9th sensor on page 3 is the number 1
    }
  }
  
  return '\0'; // Return null if nothing matches
}
