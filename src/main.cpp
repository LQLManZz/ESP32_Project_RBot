/**
 * @file main.cpp
 * @brief Entry point for the ESP32 RBot interactive face controller.
 *
 * This file handles the main Arduino setup and loop. It continuously updates
 * the OLED display based on the last received command from the serial port.
 */

#include <Arduino.h>
#include "../include/interactive_faces.h"

// Global variable to store the current facial expression/mood
String currentMood = "neutral";

/**
 * @brief Standard Arduino setup function.
 * Initializes serial communication and the OLED display.
 */
void setup()
{
  // Initialize Serial at 115200 baud for debugging and command input
  Serial.begin(115200);

  // Initialize the OLED display screen
  initDisplay();

  // Show the initial face
  showFace(currentMood);
}

/**
 * @brief Standard Arduino loop function.
 * Continuously listens for new commands and updates the screen animation.
 */
void loop()
{
  // Check if there is data available in the serial buffer to change the mood
  if (Serial.available() > 0)
  {
    // Read the incoming string until a newline character
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any leading or trailing whitespace

    // Only update if the command is not empty
    if (command.length() > 0)
    {
      currentMood = command;
    }
  }

  // Continuously update the face
  showFace(currentMood);
}