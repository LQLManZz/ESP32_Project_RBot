/**
 * @file main.cpp
 * @brief Diem bat dau va vong lap dieu khien cho he thong khuon mat tuong tac RBot ESP32.
 *
 * Tap tin nay thuc hien logic chinh cho du an RBot.
 * Cac trach nhiem chinh bao gom:
 * 1. Khoi tao phan cung (Serial va OLED I2C).
 * 2. Lang nghe cac chuoi lenh bat dong bo qua cong Serial.
 * 3. Lam sach va xu ly cac lenh de cap nhat "tam trang" cua robot.
 * 4. Duy tri chu ky lam moi hien thi lien tuc de ho tro hoat anh.
 */

#include "../include/interactive_faces.h"
#include "../include/interactive_voices.h"

/**
 * @var currentMood
 * @brief Bien trang thai toan cuc dai dien cho bieu cam hien tai cua robot.
 *
 * Chuoi nay dong vai tro la cau noi giua lop giao tiep (Serial)
 * va lop hien thi (OLED). Ham 'showFace' su dung chuoi nay
 * de quyet dinh se ve gi tren moi vong lap.
 *
 * Mac dinh: "neutral" (Kich hoat hoat anh bitmap)
 */
String currentMood = "neutral";
String currentMoodforVoice = "neutral";

/**
 * @brief Thiet lap he thong va khoi tao ngoai vi.
 *
 * Ham nay chi chay mot lan khi ESP32 duoc cap nguon hoac reset.
 * No dam bao tat ca cac thanh phan phan cung da san sang cho vong lap ung dung chinh.
 */
void setup()
{
  /**
   * Khoi tao giao tiep Serial.
   * Toc do baud: 115200 (Chuan cho ESP32)
   * Su dung: Xuat thong tin go loi va nhan cac lenh tam trang tu PC hoac MCU khac.
   */
  Serial.begin(115200);
  Serial.setTimeout(10);

  /**
   * Khoi tao man hinh OLED.
   * Goi logic trong interactive_faces.cpp de bat dau bus I2C va
   * xac nhan man hinh dang phan hoi.
   */
  initDisplay();
  initVoice();
}

/**
 * @brief Vong lap thuc thi chinh.
 *
 * Ham nay chay lap di lap lai va chua logic ung dung cot loi.
 * No duoc thiet ke de khong gay nghen (non-blocking) de dam bao hoat anh muot ma.
 */
void loop()
{
  /**
   * Buoc 1: Kiem tra cac lenh ben ngoai (Giao tiep Serial)
   *
   * Su dung 'Serial.available()' de kiem tra xem co byte nao dang cho trong bo dem khong.
   * Dieu nay ngan chuong trinh bi treo khi cho du lieu.
   */
  if (Serial.available() > 0)
  {
    /**
     * Doc chuoi du lieu den cho den khi tim thay ky tu dong moi ('\n').
     * Huu ich cho cac lenh gui tu Serial monitor hoac cac cong cu terminal.
     */
    String command = Serial.readStringUntil('\n');

    /**
     * Lam sach du lieu:
     * Loai bo moi khoang trang dau/cuoi, ky tu xuong dong (\r), hoac
     * khoang trang thua co the gay can tro viec so sanh chuoi trong showFace().
     */
    command.trim();

    /**
     * Xu ly lenh:
     * Chi cap nhat 'currentMood' toan cuc neu chuoi ket qua co noi dung.
     */
    if (command.length() > 0)
    {
      currentMood = command;
      currentMoodforVoice = command;

      /**
       * Phan hoi:
       * Gui xac nhan lai qua Serial de nguoi dung biet lenh
       * da duoc nhan va ap dung thanh cong.
       */
      Serial.print("Mood changed - ");
      Serial.println(currentMood);

      // Chung minh Core 1 dang xu ly giao dien nguoi dung (UI)
      Serial.print("[DISPLAY] Dang ve hoat anh tren CORE: ");
      Serial.println(xPortGetCoreID());
    }
  }

  /**
   * Buoc 2: Lam moi man hinh
   *
   * Goi 'showFace' tren moi vong lap duy nhat.
   * - Neu tam trang la tinh (nhu "happy"), no ve lai cac ky tu tuong tu.
   * - Neu tam trang la hoat anh (nhu "neutral"), 'showFace' uy quyen cho
   *   'makeMotion', ham nay tu quan ly thoi gian noi bo (200ms) de cap nhat
   *   cac khung hinh bitmap.
   */
  showFace(currentMood);
  if (currentMoodforVoice.length() > 0)
  {
    speakSentence(currentMoodforVoice);
    currentMoodforVoice = "";
  }
}
