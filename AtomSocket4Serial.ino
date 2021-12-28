#define NAME "AtomSocket2Serial v1.2"
#define WDT_TIMEOUT_MS 2000
//#define ENABLE_WDT

#include "M5Atom.h"
#include "AtomSocket.h"
#include <WiFi.h>
#ifdef ENABLE_WDT
#include "esp_system.h"
#endif /* ENABLE_WDT */

// ピン
#define PIN_RXD 22
#define PIN_RELAY 23

// 色
#define COLOR_RED 0xff0000
#define COLOR_GREEN 0x00ff00
#define COLOR_WHITE 0xffffff

ATOMSOCKET AtomSocket;
HardwareSerial AtomSerial(2);
#ifdef ENABLE_WDT
hw_timer_t *timer = NULL;
#endif /* ENABLE_WDT */

// ウォッチドッグタイマー割り込み
void IRAM_ATTR resetModule() {
  Serial.print("WDT\n\r");
  delay(50);
  esp_restart(); // リセット
}

void setup() {
  M5.begin(true, false, true);
  WiFi.mode(WIFI_OFF);
  Serial.begin(9600);

  // 電源関係
  AtomSocket.Init(AtomSerial, PIN_RELAY, PIN_RXD);
  delay(50);
  AtomSocket.SetPowerOff(); // 初期状態はPowerOFF

  // 初回表示
  M5.dis.drawpix(0, COLOR_RED);
  Serial.print("\n\r");
  Serial.print(NAME);
  Serial.print("\n\r");

#ifdef ENABLE_WDT
  // ウォッチドッグタイマー関連
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, WDT_TIMEOUT_MS * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
#endif /* ENABLE_WDT */
}

void loop() {
  static bool isPowerOn = false; // 電源状態
  static char buff[4];   // 格納用文字列
  static int buffCount = 0;  // 文字数のカウンタ
  static int color = COLOR_RED; // 色
  static uint64_t readTime = 0;
  static float readV = 0.0f;
  static float readA = 0.0f;
  static float readW = 0.0f;
  bool isUpdate = false; // 更新の有無

#ifdef ENABLE_WDT
  //ウォッチドッグタイマーリセット
  timerWrite(timer, 0);
#endif /* ENABLE_WDT */
  // M5Stack処理更新
  M5.update();

  // シリアル処理
  int len = Serial.available();
  for (int count = 0; count < len; count++) {
    char c = Serial.read();
    Serial.print(c);
    if (c == '\r') {
      buff[buffCount] = '\0';
      String str = String(buff);
      buffCount = 0;

      if (str == "ON") {
        isPowerOn = true;
        isUpdate = true;
      }
      else if (str == "OFF") {
        isPowerOn = false;
        isUpdate = true;
      }
      else if (str == "VER") {
        Serial.print("\n\rOK\n\r");
        Serial.print(NAME);
        Serial.print("\n\r");
      }
      else if (str == "RST") {
        Serial.print("\n\rOK\n\r");
        delay(50);
        esp_restart(); // リセット
      }
      else if (str == "GET") {
        if (isPowerOn) {
          Serial.print("\n\rOK\n\rON\n\r");
        } else {
          Serial.print("\n\rOK\n\rOFF\n\r");
        }
      }
      else if (str == "MAN") {
        Serial.print("\n\rOK\n\rON:電源をONにする\n\rOFF:電源をOFFにする\n\rGET:電源の状態(ON/OFF)を表示する\n\rRST:M5Atomをリセットする\n\rVER:ソフトバージョンを表示する\n\r");
      }
    } else if (buffCount == 3) {
      buffCount = 0;
    } else {
      buff[buffCount++] = c;
    }
  }

  // ボタン処理
  if (M5.Btn.wasPressed()) {
    isPowerOn = !isPowerOn;
    isUpdate = true;
    Serial.print("\n\rOK\n\r");
  }

  // 電源更新処理
  if (isUpdate) {
    M5.dis.drawpix(0, COLOR_WHITE); // 白色にする
    if (isPowerOn) {
      AtomSocket.SetPowerOn();
      color = COLOR_GREEN;
    } else {
      AtomSocket.SetPowerOff();
      color = COLOR_RED;
    }
    delay(50);
    Serial.print("\n\rOK\n\r");
  }

  // 時間によるLED輝度変更
  if ((millis() / 1000) % 2 == 0) {
    M5.dis.drawpix(0, color);
  } else {
    M5.dis.drawpix(0, color & 0x222222);
  }
}
