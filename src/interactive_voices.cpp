#include "../include/interactive_voices.h"
#include <AudioOutputI2S.h>
#include <ESP8266SAM.h>

// Con tro cho dau ra am thanh va bo tong hop giong noi
AudioOutputI2S *out;
ESP8266SAM *sam;

// 1. Dinh nghia handle cho hang doi noi bo
QueueHandle_t audioQueue;

// 2. Dinh nghia nhiem vu nen cho Core 0
void audioTask(void *parameter)
{
    char sentenceToSpeak[100];

    for (;;)
    {
        // Kien nhanh cho doi mot tin nhan xuat hien trong hang doi
        if (xQueueReceive(audioQueue, &sentenceToSpeak, portMAX_DELAY))
        {

            // 1. Yeu cau Watchdog ngung theo doi Core 0
            disableCore0WDT();

            // Chung minh Core 0 dang thuc hien tinh toan
            Serial.print("[AUDIO] Dang tong hop giong noi tren CORE: ");
            Serial.println(xPortGetCoreID());

            // 2. Thuc hien tinh toan de tao ra giong noi
            sam->Say(out, sentenceToSpeak);

            // Tra ve so luong BYTE con lai trong bo dem bo nho cua nhiem vu
            UBaseType_t remainingMemory = uxTaskGetStackHighWaterMark(NULL);
            Serial.print("[CORE 0] Bo nho con lai (bytes): ");
            Serial.println(remainingMemory);

            // 3. Bat lai Watchdog sau khi hoan tat
            enableCore0WDT();
        }
    }
}

void initVoice()
{
    // 1. Khoi tao dau ra phan cung I2S
    out = new AudioOutputI2S();

    // 2. Tuyet duong cac tin hieu I2S den cac chan ESP32 cu the
    // Tham so: BCLK (Clock), LRC (Word Select), DOUT (Data Out)
    out->SetPinout(26, 25, 27);

    // Dat am luong thanh 75% (0.75) de ngan chan su tang vot nang luong.
    // Thang do tu 0.0 den 1.0
    out->SetGain(0.75);

    // 3. Bat luong dau ra
    out->begin();

    // 4. Khoi tao cong cu Chuyen van ban thanh giong noi (TTS)
    sam = new ESP8266SAM();

    Serial.println("He thong am thanh duoc khoi tao tren cac chan 25, 26, 27.");

    // Tao hang doi hop thu (chua toi da 5 cau, moi cau toi da 100 ky tu)
    audioQueue = xQueueCreate(5, sizeof(char[100]));

    // Bat dau nhiem vu am thanh chuyen dung tren Core 0
    xTaskCreatePinnedToCore(
        audioTask,
        "AudioTask",
        10000,
        NULL,
        1,
        NULL,
        0 // Gan vao Core 0
    );

    Serial.println("He thong am thanh va nhiem vu Core 0 da duoc khoi tao.");
}

void speakSentence(String mood)
{
    // Chuyen doi chuoi Arduino String thanh mang ky tu C tieu chuan
    char msg[150];

    if (mood == "neutral")
    {
        String text = "Hello my friend. What do you want me to do.";
        text.toCharArray(msg, 150);
        // Dua tin nhan vao hang doi de Core 0 co the tiep nhan.
        // Core 1 se tiep tuc thuc thi ngay lap tuc sau dong nay!
    }
    else if (mood == "sus")
    {
        String text = "What is that.";
        text.toCharArray(msg, 150);
    }
    else if (mood == "angry")
    {
        String text = "You want to play. Let's play.";
        text.toCharArray(msg, 150);
    }
    else if (mood == "rizz")
    {
        String text = "Hey baby. You look sick. Suck my dick. Now";
        text.toCharArray(msg, 150);
    }
    else
    {
        String text = "The fuck you want.";
        text.toCharArray(msg, 150);
    }
    xQueueSend(audioQueue, &msg, 0);
}
