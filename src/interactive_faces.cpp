/**
 * @file interactive_faces.cpp
 * @brief Logic and implementation for OLED-based facial expressions and animations.
 *
 * This file manages the Adafruit_SSD1306 display instance and provides high-level
 * functions to render both static text-based "faces" and frame-by-frame bitmap
 * animations. It handles timing for animations using a non-blocking millis()
 * approach and manages memory-efficient bitmap storage via PROGMEM.
 */

#include "../include/interactive_faces.h"
#include "../include/faces_bitmaps.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/**
 * @def SCREEN_WIDTH
 * @brief The horizontal resolution of the OLED display in pixels.
 */
#define SCREEN_WIDTH 128

/**
 * @def SCREEN_HEIGHT
 * @brief The vertical resolution of the OLED display in pixels.
 */
#define SCREEN_HEIGHT 64

/**
 * @var currentFrame
 * @brief Tracks the index of the current bitmap frame being displayed in an animation loop.
 */
int currentFrame = 0;

/**
 * @var lastFrameTime
 * @brief Stores the timestamp (in milliseconds) of when the last animation frame was rendered.
 * Used to calculate the delay between frames for smooth animation.
 */
unsigned long lastFrameTime = 0;

/**
 * @var lastMood
 * @brief Keeps track of the previous mood string to detect when a user changes the command.
 * This allows the animation system to reset to the first frame when a new mood is selected.
 */
String lastMood = "";

/**
 * @brief The display driver instance.
 *
 * Configured for a 128x64 screen using the Wire (I2C) library.
 * The '-1' parameter indicates that the OLED does not have a dedicated reset pin connected to the ESP32.
 */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * @brief Initializes the OLED hardware and communication bus.
 *
 * This function attempts to start the I2C communication with the SSD1306 controller.
 * If the display is not found (usually due to wiring or incorrect I2C address),
 * it prints an error to the Serial monitor and halts the program.
 */
void initDisplay()
{
    // Initialize the SSD1306 display using the internal charge pump (SSD1306_SWITCHCAPVCC)
    // The standard I2C address for these modules is 0x3C.
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        // If the display doesn't respond, we cannot proceed.
        Serial.println(F("CRITICAL ERROR: SSD1306 OLED not found. Check wiring/address."));
        for (;;)
            ; // Infinite loop to prevent further execution
    }

    // Prepare the screen with a basic "Ready" message
    display.clearDisplay();              // Wipe the internal buffer
    display.setTextSize(2);              // Set text scale (2x original size)
    display.setTextColor(SSD1306_WHITE); // Set pixel color to white (on)
    display.setCursor(20, 20);           // Position the text cursor
    display.print("System OK");          // Write text to the buffer
    display.display();                   // Commit the buffer to the physical screen
}

/**
 * @brief Manages timed frame transitions for bitmap animations.
 *
 * This is the core animation engine. It uses non-blocking logic to check if
 * enough time has passed to show the next frame. It also handles reading
 * bitmap data from Program Memory (PROGMEM).
 *
 * @param moodFrameArray An array of pointers to the individual frame bitmaps.
 * @param totalFrame The number of frames in the provided animation array.
 * @param mood The current mood string, used to detect if the animation needs a reset.
 */
void makeMotion(const unsigned char *const moodFrameArray[], int totalFrame, String mood)
{
    unsigned long currentMillis = millis(); // Get elapsed time since startup

    // Logic for State Reset:
    // If the mood has changed since the last call, reset the animation to frame 0.
    if (mood != lastMood)
    {
        currentFrame = 0;  // Start at the beginning of the new animation
        lastMood = mood;   // Update the state tracker
        lastFrameTime = 0; // Force the first frame to render immediately
    }

    // Non-blocking Timer Logic:
    // Check if 200 milliseconds have passed since the last frame was rendered.
    if (currentMillis - lastFrameTime > 200)
    {
        lastFrameTime = currentMillis; // Update the timestamp for the next cycle

        display.clearDisplay(); // Prepare buffer for the new frame

        // Memory Access Logic:
        // Since moodFrameArray is an array of pointers stored in PROGMEM,
        // we must use pgm_read_ptr to retrieve the actual memory address of the bitmap.
        const unsigned char *bitmap = (const unsigned char *)pgm_read_ptr(&moodFrameArray[currentFrame]);

        // Rendering Logic:
        // The bitmaps are 77x64px.
        // To center horizontally on a 128px screen: (128 - 77) / 2 = 25.5 -> 25.
        // parameters: (x, y, bitmap_data, width, height, color)
        display.drawBitmap(25, 0, bitmap, 77, 64, SSD1306_WHITE);

        display.display(); // Update physical screen with the new bitmap frame

        // Loop Management:
        // Move to the next frame index. Reset to 0 if we've reached the end of the array.
        currentFrame++;
        if (currentFrame >= totalFrame)
        {
            currentFrame = 0;
        }
    }
}

/**
 * @brief High-level dispatcher that maps mood strings to specific rendering actions.
 *
 * This is the main interface used by main.cpp. It decides whether to draw
 * static text or trigger a bitmap animation sequence based on the input mood.
 *
 * @param mood The command string received via Serial (e.g., "happy", "neutral").
 */
void showFace(String mood)
{
    // Check the mood string and execute the corresponding drawing logic
    if (mood == "angry")
    {
        makeMotion(angry_face, angry_face_totalFrames, mood);
    }
    else if (mood == "sus")
    {
        makeMotion(sus_face, sus_face_totalFrames, mood);
    }
    else if (mood == "neutral")
    {
        // Trigger the multi-frame bitmap animation for the neutral expression.
        // The bitmaps and frame count are defined in bitmaps.h.
        makeMotion(neutral_face, neutral_face_totalFrames, mood);
    }
    else
    {
        // Error handling for unrecognized commands:
        // Clear the screen and display the received "unknown" string for debugging.
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.print("Whatchu mean " + mood + " ??");
    }

    // Final push to ensure any changes in the buffer are reflected on the OLED.
    display.display();
}
