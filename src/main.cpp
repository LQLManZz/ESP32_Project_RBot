#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  // Start serial communication at 115200 baud rate
  Serial.begin(115200);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever'
  }

  // Clear the buffer and show a startup message
  display.clearDisplay();
  display.setTextSize(3);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(10, 20);           // Start at top-left corner
  display.println("Ready!");
  display.display();

  Serial.println("Terminal ready. Send h, s, a, or w.");
}

void loop()
{
  // Check if data is available in the Serial buffer
  if (Serial.available() > 0)
  {
    char input = Serial.read(); // Read the incoming byte

    // Ignore newline and carriage return characters from the terminal
    if (input == '\n' || input == '\r')
    {
      return;
    }

    // Clear the screen for the new emoji
    display.clearDisplay();
    display.setCursor(20, 20);

    // Determine which emoji to show based on the input character
    switch (input)
    {
    case 'h':
      display.println("^_^"); // Happy
      break;
    case 's':
      display.println("T_T"); // Sad
      break;
    case 'a':
      display.println(">_<"); // Angry
      break;
    case 'w':
      display.println("O_O"); // Wow
      break;
    default:
      display.println("?"); // Unknown input
      break;
    }

    // Push the changes to the screen
    display.display();
  }
}