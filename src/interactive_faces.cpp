/*
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

// Create an instance of the display object with screen dimensions and I2C setup
// -1 means no shared reset pin is used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * Initializes the OLED display and shows a startup message.
 * Sets up I2C communication and checks for display connectivity.
 */
void initDisplay()
{
    // Initialize the screen using its I2C address (usually 0x3C)
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
 * @brief Updates the OLED screen to display a specific expression.
 * @param mood The desired facial expression as a String (e.g., "happy", "sad").
 */
void showFace(String mood)
{
    // Clear the display for the new expression
    display.clearDisplay();
    display.setCursor(35, 20);
    display.setTextSize(3); // Set large text size for the "face" characters

    // Map mood strings to character-based facial expressions
    if (mood == "happy")
    {
        display.print("^_^");
    }
    else if (mood == "sad")
    {
        display.print("T_T");
    }
    else if (mood == "alert")
    {
        display.print("O_O");
    }
    else if (mood == "neutral")
    {
        display.print("._.");
    }
    else
    {
        // Default fallback if the command is unrecognized
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.print("Unrecognized: " + mood);
    }

    // Commit changes to the physical display
    display.display();
}