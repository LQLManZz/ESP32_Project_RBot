/**
 * @file interactive_faces.cpp
 * @brief Logic va thuc thi cho cac bieu cam khuon mat va hoat anh dua tren OLED.
 *
 * Tep nay quan ly thuc the hien thi Adafruit_SSD1306 va cung cap cac ham cap cao
 * de hien thi ca "khuon mat" dua tren van ban tinh va hoat anh bitmap tung khung hinh.
 * No xu ly thoi gian cho hoat anh bang cach su dung cach tiep can millis() khong nghen
 * va quan ly viec luu tru bitmap tiet kiem bo nho thong qua PROGMEM.
 */

#include "../include/interactive_faces.h"
#include "../include/faces_bitmaps.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/**
 * @def SCREEN_WIDTH
 * @brief Do phan giai ngang cua man hinh OLED tinh bang pixel.
 */
#define SCREEN_WIDTH 128

/**
 * @def SCREEN_HEIGHT
 * @brief Do phan giai doc cua man hinh OLED tinh bang pixel.
 */
#define SCREEN_HEIGHT 64

/**
 * @var currentFrame
 * @brief Theo doi chi so cua khung hinh bitmap hien tai dang duoc hien thi trong vong lap hoat anh.
 */
int currentFrame = 0;

/**
 * @var lastFrameTime
 * @brief Luu tru dau thoi gian (tinh bang miligiay) khi khung hinh hoat anh cuoi cung duoc ve.
 * Duoc su dung de tinh toan do tre giua cac khung hinh cho hoat anh muot ma.
 */
unsigned long lastFrameTime = 0;

/**
 * @var lastMood
 * @brief Theo doi chuoi tam trang truoc do de phat hien khi nguoi dung thay doi lenh.
 * Dieu nay cho phep he thong hoat anh reset ve khung hinh dau tien khi mot tam trang moi duoc chon.
 */
String lastMood = "";

/**
 * @brief Thuc the trinh dieu khien hien thi.
 *
 * Duoc cau hinh cho man hinh 128x64 su dung thu vien Wire (I2C).
 * Tham so '-1' cho biet OLED khong co chan reset rieng biet ket noi voi ESP32.
 */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * @brief Khoi tao phan cung OLED va bus giao tiep.
 *
 * Ham nay co gang bat dau giao tiep I2C voi bo dieu khien SSD1306.
 * Neu khong tim thay man hinh (thuong do day dan hoac dia chi I2C sai),
 * no se in thong bao loi ra Serial monitor va dung chuong trinh.
 */
void initDisplay()
{
    // Khoi tao man hinh SSD1306 su dung bom dien noi bo (SSD1306_SWITCHCAPVCC)
    // Dia chi I2C tieu chuan cho cac module nay la 0x3C.
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        // Neu man hinh khong phan hoi, chung ta khong the tiep tuc.
        Serial.println(F("OLED SSD1306 not found!"));
        for (;;)
            ; // Vong lap vo han de ngan thuc thi them
    }

    // Chuan bi man hinh voi thong bao "Ready" co ban
    display.clearDisplay();              // Xoa bo dem noi bo
    display.setTextSize(2);              // Dat kich thuoc van ban (gap 2 lan kich thuoc goc)
    display.setTextColor(SSD1306_WHITE); // Dat mau pixel thanh trang (bat)
    display.setCursor(20, 20);           // Dat vi tri con tro van ban
    display.print("System OK");          // Viet van ban vao bo dem
    display.display();                   // Cap nhat bo dem len man hinh vat ly
}

/**
 * @brief Quan ly viec chuyen doi khung hinh theo thoi gian cho hoat anh bitmap.
 *
 * Day la cong cu hoat anh cot loi. No su dung logic khong nghen de kiem tra xem
 * da du thoi gian de hien thi khung hinh tiep theo chua. No cung xu ly viec doc
 * du lieu bitmap tu Bo nho chuong trinh (PROGMEM).
 *
 * @param moodFrameArray Mot mang cac con tro den cac bitmap khung hinh rieng le.
 * @param totalFrame So luong khung hinh trong mang hoat anh duoc cung cap.
 * @param mood Chuoi tam trang hien tai, duoc su dung de phat hien neu hoat anh can reset.
 */
void makeMotion(const unsigned char *const moodFrameArray[], int totalFrame, String mood)
{
    unsigned long currentMillis = millis(); // Lay thoi gian da troi qua ke tu khi khoi dong

    // Logic de Reset trang thai:
    // Neu tam trang da thay doi ke tu lan goi cuoi cung, reset hoat anh ve khung hinh 0.
    if (mood != lastMood)
    {
        currentFrame = 0;  // Bat dau tai thoi diem bat dau cua hoat anh moi
        lastMood = mood;   // Cap nhat trinh theo doi trang thai
        lastFrameTime = 0; // Buoc khung hinh dau tien phai hien thi ngay lap tuc
    }

    // Logic Bo dem thoi gian khong nghen:
    unsigned long frameTime = rand() % 301 + 200;
    if (currentMillis - lastFrameTime > frameTime)
    {
        lastFrameTime = currentMillis; // Cap nhat dau thoi gian cho chu ky tiep theo

        display.clearDisplay(); // Chuan bi bo dem cho khung hinh moi

        // Logic Truy cap Bo nho:
        // Vi moodFrameArray la mot mang cac con tro duoc luu tru trong PROGMEM,
        // chung ta phai su dung pgm_read_ptr de lay dia chi bo nho thuc cua bitmap.
        const unsigned char *bitmap = (const unsigned char *)pgm_read_ptr(&moodFrameArray[currentFrame]);

        // Logic Hien thi:
        // Cac bitmap co kich thuoc 77x64px.
        // De can giua ngang tren man hinh 128px: (128 - 77) / 2 = 25.5 -> 25.
        // tham so: (x, y, bitmap_data, width, height, color)
        display.drawBitmap(25, 0, bitmap, 77, 64, SSD1306_WHITE);

        display.display(); // Cap nhat man hinh vat ly voi khung hinh bitmap moi

        // Quan ly Vong lap:
        // Chuyen sang chi so khung hinh tiep theo. Reset ve 0 neu chung ta da den cuoi mang.
        currentFrame++;

        // Serial.print("Frame hien tai: ");
        // Serial.println(currentFrame);

        if (currentFrame >= totalFrame)
        {
            currentFrame = 0;
        }
    }
}

/**
 * @brief Bo dieu phoi cap cao anh xa cac chuoi tam trang den cac hanh dong hien thi cu the.
 *
 * Day la giao dien chinh duoc su dung boi main.cpp. No quyet dinh nen ve
 * van ban tinh hay kich hoat chuoi hoat anh bitmap dua tren tam trang dau vao.
 *
 * @param mood Lenh chuoi nhan duoc qua Serial (vi du: "happy", "neutral").
 */
void showFace(String mood)
{
    // Kiem tra chuoi tam trang va thuc hien logic ve tuong ung
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
        // Kich hoat hoat anh bitmap nhieu khung hinh cho bieu cam trung tinh.
        // Cac bitmap va so luong khung hinh duoc dinh nghia trong bitmaps.h.
        makeMotion(neutral_face, neutral_face_totalFrames, mood);
    }
    else if (mood == "rizz")
    {
        makeMotion(neutral_face, neutral_face_totalFrames, mood);
    }
    else if (mood == "cry")
    {
        makeMotion(cry_face, cry_face_totalFrames, mood);
    }
    else
    {
        // Xu ly loi cho cac lenh khong xac dinh:
        // Xoa man hinh va hien thi chuoi "unknown" nhan duoc de go loi.
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.print("Whatchu mean " + mood + " ??");
    }

    // Day cuoi cung de dam bao moi thay doi trong bo dem deu duoc phan chieu tren OLED.
    display.display();
}
