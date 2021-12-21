#define NAME "AtomSocket2Serial v1.1"
// ウォッチドッグタイマー設定
//#define WDT_ENABLE

#include "M5Atom.h"
#include "AtomSocket.h"
#ifdef WDT_ENABLE
#include "esp_system.h"
#define WDT_TIMEOUT_MS 500
#endif /* WDT_ENABLE */

// ピン
#define PIN_RXD 22
#define PIN_RELAY 23

// LEDの色
#define COLOR_OFF    0x00ff00 // OFF時
#define COLOR_ON     0xff0000 // ON時
#define COLOR_UPDATE 0xffffff // 更新時

ATOMSOCKET AtomSocket;
HardwareSerial AtomSerial(2);
bool isPowerOn = false; // 電源状態
bool isUpdate = false; // 更新の有無

#ifdef WDT_ENABLE
hw_timer_t *timer = NULL;
#endif /* WDT_ENABLE */

#ifdef WDT_ENABLE
// ウォッチドッグタイマー割り込み
void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart(); // リセット
}
#endif /* WDT_ENABLE */

// パワーONにする
void setPowerOn() {
  isPowerOn = true;
  isUpdate = true;
}

// パワーOFFにする
void setPowerOff() {
  isPowerOn = false;
  isUpdate = true;
}

void setup() {
  M5.begin(true, false, true);
  Serial.begin(9600);

  // Socket関係処理
  AtomSocket.Init(AtomSerial, PIN_RELAY, PIN_RXD);
  delay(50);
  AtomSocket.SetPowerOff(); // 初期状態はPowerOFF

  // 初回表示
  M5.dis.drawpix(0, COLOR_OFF);
  Serial.print("\n\r");
  Serial.print(NAME);
  Serial.print("\n\r");

#ifdef WDT_ENABLE
  // ウォッチドッグタイマー関連
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, WDT_TIMEOUT_MS * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
#endif /* WDT_ENABLE */
}

void loop() {
  static int color = COLOR_OFF; // LEDの色
  static char buff[4];          // シリアル受信格納用
  static int buffCount = 0;     // シリアル受信カウンター

#ifdef WDT_ENABLE
  //ウォッチドッグタイマーリセット
  timerWrite(timer, 0);
#endif /* WDT_ENABLE */

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
        setPowerOn();
      }
      else if (str == "OFF") {
        setPowerOff();
      }
      else if (str == "VER") {
        Serial.print("\n\r");
        Serial.print(NAME);
        Serial.print("\n\r");
      }
      else if (str == "RST") {
        Serial.print("OK\n\r");
        delay(50);
        esp_restart(); // リセット
      }
      else if (str == "GET") {
        if (isPowerOn) {
          Serial.print("\n\rON\n\rOK\n\r");
        } else {
          Serial.print("\n\rOFF\n\rOK\n\r");
        }
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
      setPowerOn();
    } else {
      setPowerOff();
    }
  }

  // 電源更新処理
  if (isUpdate) {
    M5.dis.drawpix(0, COLOR_UPDATE);
    if (isPowerOn) {
      AtomSocket.SetPowerOn();
      color = COLOR_ON;
    } else {
      AtomSocket.SetPowerOff();
      color = COLOR_OFF;
    }
    delay(50);
    Serial.print("\n\rOK\n\r");
  }

  // 時間によるLED輝度変更
  if ((millis() / 1000) % 2 == 0) {
    M5.dis.drawpix(0, color);
  } else {
    M5.dis.drawpix(0, color & 0x444444);
  }
}
