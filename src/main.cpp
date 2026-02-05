#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Keyboard.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PN532.h>
#include <Adafruit_Fingerprint.h>

#include "pins.h"

// =========================
//  OLED
// =========================
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);

// =========================
//  PN532 (SPI - software SPI)
// =========================
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// =========================
//  Fingerprint (AS608) - UART (Serial1)
// =========================
Adafruit_Fingerprint finger(&Serial1);

// =========================
//  UI State
// =========================
enum UIMode {
  MODE_MENU = 0,
  MODE_FINGERPRINT,
  MODE_EKTP
};

UIMode uiMode = MODE_MENU;
int menuIndex = 0; // 0 = Fingerprint, 1 = eKTP

// =========================
//  Tombol (MODE & SELECT = edge; BACK = level)
// =========================
bool lastModeState   = true;
bool lastSelectState = true;

bool modePressedEdge   = false;
bool selectPressedEdge = false;
bool backPressed       = false; // LEVEL

void readButtons() {
  bool modeNow   = digitalRead(BTN_MODE);
  bool selectNow = digitalRead(BTN_SELECT);
  bool backNow   = digitalRead(BTN_BACK);

  // MODE & SELECT edge
  modePressedEdge   = (lastModeState   == HIGH && modeNow   == LOW);
  selectPressedEdge = (lastSelectState == HIGH && selectNow == LOW);

  lastModeState   = modeNow;
  lastSelectState = selectNow;

  // BACK level
  backPressed = (backNow == LOW);
}

// =========================
//  Buzzer helper
// =========================
void buzzerBeep(uint16_t ms) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(ms);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzerSuccess() { buzzerBeep(120); }
void buzzerFailOnce() { buzzerBeep(200); }

// =========================
//  OLED helper
// =========================
void oledText3(const char *l1, const char *l2 = "", const char *l3 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  if (l1) display.println(l1);

  if (l2 && l2[0]) {
    display.setCursor(0, 16);
    display.println(l2);
  }

  if (l3 && l3[0]) {
    display.setCursor(0, 32);
    display.println(l3);
  }

  display.display();
}

void showSplash() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(5, 0);
  display.println("SUPANGAT");

  display.setTextSize(1);
  display.setCursor(10, 24);
  display.println("Biometric Kit");

  display.setCursor(10, 40);
  display.println("V. 5.1");

  display.display();

  buzzerBeep(1000);
  digitalWrite(BUZZER_PIN, LOW);
}

void drawMainMenu() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("Menu:");

  display.setCursor(0, 16);
  display.print(menuIndex == 0 ? "> " : "  ");
  display.println("Fingerprint");

  display.setCursor(0, 28);
  display.print(menuIndex == 1 ? "> " : "  ");
  display.println("eKTP / NFC");

  display.display();
}

void drawFingerprintScreen(const char* status) {
  oledText3("Fingerprint", status, "Data via HID + TAB");
}

void drawEKTPscreen(const char* status) {
  oledText3("eKTP / NFC", status, "UID via HID + TAB");
}

// =========================
//  Helper Hex + HID
// =========================
String bytesToHexString(const uint8_t *data, uint16_t len) {
  const char *hex = "0123456789ABCDEF";
  String out; out.reserve(len * 2);

  for (uint16_t i = 0; i < len; i++) {
    uint8_t b = data[i];
    out += hex[b >> 4];
    out += hex[b & 0x0F];
  }
  return out;
}

void sendTextAndTab(const String &s) {
  for (size_t i = 0; i < s.length(); i++)
    Keyboard.write(s[i]);

  Keyboard.write(KEY_TAB);
}

uint16_t trimTrailingZeros(uint8_t *data, uint16_t len) {
  while (len > 0 && data[len - 1] == 0x00)
    len--;
  return len;
}

// =========================
//  Fingerprint process
// =========================
bool processFingerprintFromCurrentImage() {
  uint8_t p;

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return false;

  p = finger.uploadCharFromBuffer(1);
  if (p != FINGERPRINT_OK) return false;

  if (!finger.downloadedTemplate || finger.downloadedTemplateLength == 0)
    return false;

  uint16_t len = trimTrailingZeros(
      finger.downloadedTemplate,
      finger.downloadedTemplateLength
  );
  if (len == 0) return false;

  String tplHex = bytesToHexString(finger.downloadedTemplate, len);
  sendTextAndTab(tplHex);

  return true;
}

// =========================
//  eKTP process (NON-BLOCKING)
// =========================
bool processEKTPOnce(uint16_t timeoutMs = 50) {
  uint8_t uid[7];
  uint8_t uidLength;

  bool success = nfc.readPassiveTargetID(
    PN532_MIFARE_ISO14443A,
    uid,
    &uidLength,
    timeoutMs   // <--- FIX: supaya tidak hang
  );

  if (!success) return false;

  String uidHex = bytesToHexString(uid, uidLength);
  sendTextAndTab(uidHex);

  return true;
}

// =========================
//  SETUP
// =========================
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(BTN_MODE,   INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_BACK,   INPUT_PULLUP);

  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR))
    while (true) delay(1000);

  showSplash();

  Serial1.begin(FP_BAUDRATE);
  delay(200);
  finger.begin(FP_BAUDRATE);
  delay(200);

  if (!finger.verifyPassword()) {
    oledText3("FP ERROR", "AS608 tidak terdeteksi", "Cek TX/RX/3V3/GND");
    while (true) delay(1000);
  }

  nfc.begin();
  delay(200);
  if (!nfc.getFirmwareVersion()) {
    oledText3("PN532 ERROR", "Tidak terdeteksi", "Cek 5V/GND/SPI");
    while (true) delay(1000);
  }
  nfc.SAMConfig();

  Keyboard.begin();

  drawMainMenu();
}

// =========================
//  LOOP
// =========================
void loop() {
  readButtons();

  switch (uiMode) {

    case MODE_MENU: {
      if (modePressedEdge) {
        menuIndex = (menuIndex + 1) % 2;
        drawMainMenu();
      }

      if (selectPressedEdge) {
        if (menuIndex == 0) {
          uiMode = MODE_FINGERPRINT;
          drawFingerprintScreen("Tempelkan Jari");
        } else {
          uiMode = MODE_EKTP;
          drawEKTPscreen("Tempelkan KTP");
        }
      }
    } break;

    case MODE_FINGERPRINT: {
      if (backPressed) {
        uiMode = MODE_MENU;
        drawMainMenu();
        break;
      }

      drawFingerprintScreen("Tempelkan Jari");

      uint8_t p = finger.getImage();
      if (p == FINGERPRINT_OK) {
        drawFingerprintScreen("Proses...");

        bool ok = processFingerprintFromCurrentImage();
        drawFingerprintScreen(ok ? "Berhasil" : "Gagal");
        if (ok) buzzerSuccess();

        delay(800);
      } else {
        delay(80);
      }
    } break;

    case MODE_EKTP: {
      // BACK selalu dicek paling awal
      if (backPressed) {
        uiMode = MODE_MENU;
        drawMainMenu();
        break;
      }

      drawEKTPscreen("Tempelkan KTP");

      if (processEKTPOnce(50)) {
        drawEKTPscreen("Berhasil");
        buzzerSuccess();
        delay(800);
      } else {
        delay(50);
      }
    } break;
  }
}
