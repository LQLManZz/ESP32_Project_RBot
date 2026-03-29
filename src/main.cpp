/**
 * @file main.cpp
 * @brief Entry point and control loop for the ESP32 RBot interactive face system.
 *
 * This file implements the main firmware logic for the RBot project. 
 * Its primary responsibilities include:
 * 1. Initializing the hardware (Serial and I2C OLED).
 * 2. Listening for asynchronous command strings over the Serial port.
 * 3. Sanitizing and processing commands to update the robot's "mood".
 * 4. Maintaining a continuous display refresh cycle to support animations.
 */

#include "../include/interactive_faces.h"

/**
 * @var currentMood
 * @brief Global state variable representing the robot's current expression.
 * 
 * This string acts as the bridge between the communication layer (Serial)
 * and the rendering layer (OLED). The 'showFace' function uses this string
 * to decide what to draw on every loop iteration.
 * 
 * Default: "neutral" (Triggering the bitmap animation)
 */
String currentMood = "neutral";

/**
 * @brief System setup and peripheral initialization.
 * 
 * This function runs exactly once when the ESP32 is powered on or reset.
 * It ensures all hardware components are ready for the main application loop.
 */
void setup()
{
  /**
   * Initialize Serial communication.
   * Baud rate: 115200 (Standard for ESP32)
   * Usage: Debugging output and receiving mood commands from a PC or another MCU.
   */
  Serial.begin(115200);

  /**
   * Initialize the OLED Display.
   * This calls the logic in interactive_faces.cpp to start the I2C bus and 
   * verify the screen is responding.
   */
  initDisplay();

  /**
   * Draw the initial face.
   * Ensures the screen isn't blank immediately after startup.
   */
  showFace(currentMood);
}

/**
 * @brief The main execution loop.
 * 
 * This function runs repeatedly and contains the core application logic.
 * It is designed to be non-blocking to ensure smooth animations.
 */
void loop()
{
  /**
   * Step 1: Check for External Commands (Serial Communication)
   * 
   * We use 'Serial.available()' to check if bytes are waiting in the buffer.
   * This prevents the program from hanging while waiting for data.
   */
  if (Serial.available() > 0)
  {
    /**
     * Read the incoming string data until a newline ('\n') is found.
     * This is useful for commands sent from Serial monitors or terminal tools.
     */
    String command = Serial.readStringUntil('\n');
    
    /**
     * Data Sanitization:
     * Remove any leading/trailing whitespace, carriage returns (\r), or 
     * extra spaces that might interfere with string comparison in showFace().
     */
    command.trim(); 

    /**
     * Command Processing:
     * Only update the global 'currentMood' if the resulting string has content.
     */
    if (command.length() > 0)
    {
      currentMood = command;
      
      /**
       * Feedback:
       * Send a confirmation back over Serial so the user knows the command
       * was successfully received and applied.
       */
      Serial.print("SUCCESS: Received command - ");
      Serial.println(currentMood);
    }
  }

  /**
   * Step 2: Refresh the Display
   * 
   * We call 'showFace' on every single iteration of the loop.
   * - If the mood is static (like "happy"), it redraws the same characters.
   * - If the mood is animated (like "neutral"), 'showFace' delegates to 
   *   'makeMotion', which handles its own internal timing (200ms) to update 
   *   the bitmap frames.
   */
  showFace(currentMood);
}
