#include "../include/interactive_voices.h"
#include <AudioOutputI2S.h>
#include <ESP8266SAM.h>

// Pointers for the audio output and the speech synthesizer
AudioOutputI2S *out;
ESP8266SAM *sam;

// 1. Define the internal queue handle
QueueHandle_t audioQueue;

// 2. Define the background task for Core 0
void audioTask(void *parameter)
{
    char sentenceToSpeak[100];

    for (;;)
    {
        // Wait patiently for a message to appear in the queue
        if (xQueueReceive(audioQueue, &sentenceToSpeak, portMAX_DELAY))
        {

            // 1. Tell the Watchdog to stop monitoring Core 0
            disableCore0WDT();

            // Prove Core 0 is doing the math
            Serial.print("[AUDIO] Synthesizing voice on CORE: ");
            Serial.println(xPortGetCoreID());

            // 2. Do the heavy math to generate the voice
            sam->Say(out, sentenceToSpeak);

            // Returns the number of BYTES left in the task's memory buffer
            UBaseType_t remainingMemory = uxTaskGetStackHighWaterMark(NULL);
            Serial.print("[CORE 0] Memory bytes remaining: ");
            Serial.println(remainingMemory);

            // 3. Turn the Watchdog back on now that we are done
            enableCore0WDT();
        }
    }
}

void initVoice()
{
    // 1. Initialize the I2S hardware output
    out = new AudioOutputI2S();

    // 2. Route the I2S signals to specific ESP32 pins
    // Parameters: BCLK (Clock), LRC (Word Select), DOUT (Data Out)
    out->SetPinout(26, 25, 27);

    // Set the volume to 10% (0.1) to prevent power spikes.
    // The scale goes from 0.0 to 1.0
    out->SetGain(0.1);

    // 3. Turn on the output stream
    out->begin();

    // 4. Initialize the Text-to-Speech engine
    sam = new ESP8266SAM();

    Serial.println("Audio system initialized on pins 25, 26, 27.");

    // Create the mailbox queue (holds up to 5 sentences of 100 characters max)
    audioQueue = xQueueCreate(5, sizeof(char[100]));

    // Start the dedicated audio task on Core 0
    xTaskCreatePinnedToCore(
        audioTask,
        "AudioTask",
        10000,
        NULL,
        1,
        NULL,
        0 // Pinned to Core 0
    );

    Serial.println("Audio system and Core 0 task initialized.");
}

void speakSentence(String mood)
{
    // Convert the Arduino String into a standard C character array
    char msg[100];

    if (mood == "neutral")
    {
        String text = "Hello World.";
        text.toCharArray(msg, 100);
        // Drop the message into the queue so Core 0 can pick it up.
        // Core 1 moves on immediately after this line!
        xQueueSend(audioQueue, &msg, 0);
    }
    else if (mood == "sus")
    {
        String text = "What is that.";
        text.toCharArray(msg, 100);
        // Drop the message into the queue so Core 0 can pick it up.
        // Core 1 moves on immediately after this line!
        xQueueSend(audioQueue, &msg, 0);
    }
    else if (mood == "angry")
    {
        String text = "You want to play. Let's play.";
        text.toCharArray(msg, 100);
        // Drop the message into the queue so Core 0 can pick it up.
        // Core 1 moves on immediately after this line!
        xQueueSend(audioQueue, &msg, 0);
    }
    else
    {
        String text = "The fuck you want.";
        text.toCharArray(msg, 100);
        // Drop the message into the queue so Core 0 can pick it up.
        // Core 1 moves on immediately after this line!
        xQueueSend(audioQueue, &msg, 0);
    }
}