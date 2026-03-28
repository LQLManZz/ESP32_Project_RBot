/**
 * @file interactive_faces.cpp
 * @brief Implementation of functions to control OLED expressions.
 *
 * This file uses the Adafruit SSD1306 library to initialize and display
 * simple textual "faces" on an I2C OLED screen.
 */

#include "../include/interactive_faces.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen dimensions for standard 128x64 OLED displays
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Animation variables
unsigned long lastBlinkTime = 0;
int blinkState = 0; // 0, 1, or 2 to track the animation frame

// Create an instance of the display object with screen dimensions and I2C setup
// -1 means no shared reset pin is used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * @brief Initializes the OLED display and shows a startup message.
 * Sets up I2C communication and checks for display connectivity.
 */
void initDisplay()
{
    // Initialize the screen using its I2C address (0x3C)
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        // Log failure message if OLED fails to start
        Serial.println(F("SSD1306 OLED allocation failed"));
        for (;;)
            ; // Enter an infinite loop to halt execution
    }

    // Clear the screen buffer and show a "System OK" message
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE); // Use SSD1306_WHITE for compatibility
    display.setCursor(20, 20);
    display.print("System OK");
    display.display(); // Push the buffer content to the screen
}

/**
 * @brief Clears the screen and draws a textual face.
 * @param text The characters representing the face.
 */
void drawFaceText(String text)
{
    display.clearDisplay();
    display.setCursor(35, 20);
    display.setTextSize(3); // Set large text size for the "face" characters
    display.print(text);
    // Important: display.display() is called in showFace() after this function returns
}

/**
 * @brief Creates a sequential animation by cycling through three mood states.
 * @param state1 First animation frame.
 * @param state2 Second animation frame.
 * @param state3 Third animation frame.
 */
void makeMotion(String state1, String state2, String state3)
{
    unsigned long currentMillis = millis(); // Get the current time in milliseconds

    // Check if it's time to change to the next animation frame (every 500ms)
    if (currentMillis - lastBlinkTime > 500)
    {
        lastBlinkTime = currentMillis; // Update the last transition time

        // Cycle through states 0, 1, 2
        blinkState = (blinkState + 1) % 3;
    }

    // Draw the current state's face text based on blinkState
    if (blinkState == 0)
    {
        drawFaceText(state1);
    }
    else if (blinkState == 1)
    {
        drawFaceText(state2);
    }
    else if (blinkState == 2)
    {
        drawFaceText(state3);
    }
}

/**
 * @brief Updates the OLED screen to display a specific expression or animation.
 * @param mood The desired facial expression or animation (e.g., "smileblink").
 */
void showFace(String mood)
{
    // Map mood strings to character-based facial expressions
    if (mood == "happy")
    {
        drawFaceText("^_^");
    }
    else if (mood == "sad")
    {
        drawFaceText("T_T");
    }
    else if (mood == "alert")
    {
        drawFaceText("O_O");
    }
    else if (mood == "neutral")
    {
        drawFaceText("._.");
    }
    else if (mood == "smileblink")
    {
        makeMotion(">vO", "OvO", "Ov<");
    }
    else if (mood == "talking")
    {
        makeMotion("^o^", "^O^", "O_O");
    }
    else
    {
        // Default fallback if the command is unrecognized
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.print("Unknown mood: " + mood);
    }

    // Final call to update the physical display screen
    display.display();
}