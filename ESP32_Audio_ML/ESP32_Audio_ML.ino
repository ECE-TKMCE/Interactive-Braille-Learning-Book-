/*
 * Requires Libraries:
 * 1. DFRobotDFPlayerMini
 * 2. Your exported Edge Impulse C++ Library (Install via Sketch -> Include Library -> Add .ZIP Library)
 */

#include <Arduino.h>
#include <HardwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include <driver/i2s.h>

// ---------------------------------------------------------
// IMPORTANT: Replace this with the actual name of your 
// Edge Impulse exported library header file!
// ---------------------------------------------------------
#include <Your_Edge_Impulse_Project_inferencing.h> 

// --- Pin Definitions ---
#define MODE_SWITCH_PIN 15

// I2S Microphone Pins (INMP441)
#define I2S_WS 25
#define I2S_SCK 26
#define I2S_SD 33

// DFPlayer Mini Serial Pins (Using Hardware Serial 1)
#define DFPLAYER_RX_PIN 18
#define DFPLAYER_TX_PIN 17

// Arduino Communication Pins (Using Hardware Serial 2)
#define ARDUINO_RX_PIN 16
#define ARDUINO_TX_PIN 4 // Not actively used, but required for setup

// --- Peripherals ---
HardwareSerial dfSerial(1);
HardwareSerial arduinoSerial(2);
DFRobotDFPlayerMini myDFPlayer;

// --- DFPlayer Audio Track Mapping ---
// Ensure your SD card has files named exactly like 0001.mp3, 0002.mp3 in an "mp3" folder.
// Track 1-26: A-Z
// Track 27-35: 1-9
#define TRACK_CORRECT 100
#define TRACK_TRY_AGAIN 101

// --- Audio Buffer for Machine Learning ---
#define SAMPLE_BUFFER_SIZE 512
short sampleBuffer[SAMPLE_BUFFER_SIZE];
bool isRecording = false;

void setup() {
  Serial.begin(115200); // PC Debugging
  
  // Setup Mode Switch
  pinMode(MODE_SWITCH_PIN, INPUT_PULLUP);

  // Setup Serial Communication with Arduino
  arduinoSerial.begin(9600, SERIAL_8N1, ARDUINO_RX_PIN, ARDUINO_TX_PIN);
  
  // Setup DFPlayer Mini
  dfSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
  Serial.println("Initializing DFPlayer...");
  if (!myDFPlayer.begin(dfSerial)) {
    Serial.println("DFPlayer Error: Check connections or SD card.");
  } else {
    myDFPlayer.volume(20); // Set volume 0-30
    Serial.println("DFPlayer Ready.");
  }

  // Setup I2S for INMP441 Microphone
  setupI2S();
  
  Serial.println("ESP32 System Ready.");
}

void loop() {
  // Check if a character was received from the Arduino
  if (arduinoSerial.available()) {
    char targetChar = arduinoSerial.read();
    
    // Ignore newline or carriage return characters
    if (targetChar == '\n' || targetChar == '\r') return;
    
    Serial.print("Arduino detected: ");
    Serial.println(targetChar);

    // Read the physical switch to determine the mode
    bool isReadMode = (digitalRead(MODE_SWITCH_PIN) == LOW); 

    if (!isReadMode) {
      // --------------------------------------------------
      // LISTEN MODE: Play the corresponding letter sound
      // --------------------------------------------------
      Serial.println("Mode: LISTEN");
      int trackNumber = getTrackForCharacter(targetChar);
      if (trackNumber > 0) {
        myDFPlayer.play(trackNumber);
      }
      
    } else {
      // --------------------------------------------------
      // READ MODE: Wait for student to speak, then run AI
      // --------------------------------------------------
      Serial.println("Mode: READ - Listening to microphone...");
      
      // Capture audio from I2S and run inference
      String predictedChar = captureAndClassifyAudio();
      
      Serial.print("AI Predicted: ");
      Serial.println(predictedChar);

      // Verify if the student said the correct letter
      if (predictedChar.length() > 0 && predictedChar[0] == targetChar) {
        Serial.println("Result: CORRECT!");
        myDFPlayer.play(TRACK_CORRECT);
      } else {
        Serial.println("Result: TRY AGAIN.");
        myDFPlayer.play(TRACK_TRY_AGAIN);
      }
    }
  }
}

// ---------------------------------------------------------
// Helper Functions
// ---------------------------------------------------------

// Maps the character to a specific MP3 track number
int getTrackForCharacter(char c) {
  if (c >= 'A' && c <= 'Z') {
    return (c - 'A') + 1; // 'A' = 1, 'B' = 2, etc.
  }
  if (c >= '1' && c <= '9') {
    return (c - '1') + 27; // '1' = 27, '2' = 28, etc.
  }
  return 0; // Invalid
}

// Configures the I2S interface for the INMP441
void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000, // Edge Impulse default audio rate
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };
  
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

// Captures audio and feeds it to the Edge Impulse model
String captureAndClassifyAudio() {
  // This function acts as a wrapper for the Edge Impulse inferencing pipeline.
  // In a full implementation, you fill the EI signal buffer with I2S data here.
  
  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_NUM_0, &sampleBuffer, sizeof(sampleBuffer), &bytesIn, portMAX_DELAY);
  
  if (result == ESP_OK) {
    // -------------------------------------------------------------------
    // EDGE IMPULSE INFERENCE LOGIC GOES HERE
    // 1. Convert sampleBuffer to Edge Impulse features array
    // 2. Call run_classifier()
    // 3. Loop through result.classification[i].value to find the highest score
    // 4. Return the result.classification[i].label
    // -------------------------------------------------------------------
    
    // Placeholder return simulating the model returning a classification label
    // return top_classification_label; 
  }
  
  return ""; 
}
