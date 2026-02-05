#ifndef PINS_H
#define PINS_H

// =========================
//  OLED 0.96" I2C (SSD1306)
// =========================
// Menggunakan I2C0 default di Pico (Earle Philhower):
// SDA = GPIO4, SCL = GPIO5
#define OLED_SDA        4
#define OLED_SCL        5
#define OLED_I2C_ADDR   0x3C
#define OLED_WIDTH      128
#define OLED_HEIGHT     64

// =========================
//  PN532 - SPI (Mode SPI)
// =========================
// Menggunakan SPI0 (software SPI di kode, tapi tetap pin ini):
// SCK  = GPIO18
// MOSI = GPIO19
// MISO = GPIO16
// SS   = GPIO17
#define PN532_SCK       18
#define PN532_MOSI      19
#define PN532_MISO      16
#define PN532_SS        17

// =========================
//  AS608 Fingerprint (UART)
// =========================
// Dipakai dengan Serial1 (Pico, Earle core):
// TX Pico (ke RX AS608) = GPIO0
// RX Pico (dari TX AS608) = GPIO1
#define FP_TX_PIN       0   // Pico TX -> AS608 RX
#define FP_RX_PIN       1   // Pico RX <- AS608 TX
#define FP_BAUDRATE     57600

// =========================
//  Tombol Navigasi (Tactile)
// =========================
// Wiring: satu sisi tombol -> GPIO, sisi lain -> GND
// Akan dipakai dengan INPUT_PULLUP
#define BTN_MODE        10  // Pindah pilihan menu
#define BTN_SELECT      11  // Pilih / OK
#define BTN_BACK        12  // Kembali

// =========================
//  Buzzer
// =========================
// Buzzer + ke GPIO, - ke GND
#define BUZZER_PIN      15
// =========================
//  Power (referensi, tidak di-pinMode)
// =========================
// VSYS (5V) : PN532, OLED, Fan
// 3V3       : AS608, tombol, buzzer
// Semua GND wajib common

#endif // PINS_H
