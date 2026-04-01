#ifndef ROBOT_VOICE_H
#define ROBOT_VOICE_H

#include <Arduino.h>

// Declare the functions that main.cpp can use
void initVoice();
void speakSentence(String text);

#endif