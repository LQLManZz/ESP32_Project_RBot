/* This file handles the main Arduino setup and loop, listening for serial
 * commands to update the robot's facial expressions.
 */

#include <Arduino.h>
#include "../include/interactive_faces.h"

// Initializes serial communication and the OLED display.
void setup()
{
  // Initialize Serial at 115200 baud for debugging and command input
  Serial.begin(115200);

  // Initialize the OLED display screen
  initDisplay();
}

/* Continuously checks for incoming serial commands to change the face.
 */
void loop()
{
  // Check if there is data available in the serial buffer
  if (Serial.available() > 0)
  {
    // Read the incoming string until a newline character
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any leading or trailing whitespace

    // Update the display based on the received command (e.g., "happy", "sad")
    showFace(command);
  }
}