#include "M5Atom.h"
#include "AtomSocket.h"
#include "esp_system.h"

// 設定
#define VERSION "1.0"
#define WDT_TIMEOUT_MS 500

// ピン
#define PIN_RXD 22
#define PIN_RELAY 23

// 色
#define COLOR_RED 0x00ff00
#define COLOR_GREEN 0xff0000
#define COLOR_WHITE 0xffffff

ATOMSOCKET AtomSocket;
HardwareSerial AtomSerial(2);
hw_timer_t *timer = NULL;

// ウォッチドッグタイマー割り込み
void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart(); // リセット
}

void setup() {
  M5.begin(true, false, true);
  Serial.begin(9600);

  // 電源関係
  AtomSocket.Init(AtomSerial, PIN_RELAY, PIN_RXD);
  delay(50);
  AtomSocket.SetPowerOff(); // 初期状態はPowerOFF
  M5.dis.drawpix(0, COLOR_RED);

  // ウォッチドッグタイマー関連
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, WDT_TIMEOUT_MS * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
}

void loop() {
  static bool isPowerOn = false; // 電源状態
  static char buff[4];   // 格納用文字列
  static int buffCount = 0;  // 文字数のカウンタ
  bool isUpdate = false; // 更新の有無

  //ウォッチドッグタイマーリセット
  timerWrite(timer, 0);

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
        Serial.print("\n\r");
        Serial.print(VERSION);
        Serial.print("\n\r");
      }
    } else if (buffCount == 3) {
      buffCount = 0;
    } else {
      buff[buffCount++] = c;
    }
  }

  // ボタン処理
  if (M5.Btn.wasPressed()) {
    if (isPowerOn) {
      isPowerOn = false;
    } else {
      isPowerOn = true;
    }
    isUpdate = true;
  }

  // 電源更新処理
  if (isUpdate) {
    int color;
    M5.dis.drawpix(0, COLOR_WHITE); // 白色にする
    if (isPowerOn) {
      AtomSocket.SetPowerOn();
      color = COLOR_GREEN;
    } else {
      AtomSocket.SetPowerOff();
      color = COLOR_RED;
    }
    delay(50);
    M5.dis.drawpix(0, color);
    Serial.print("\n\rOK\n\r");
  }
}
