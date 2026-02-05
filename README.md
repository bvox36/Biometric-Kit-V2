# SUPANGAT Biometric Kit  
**Fingerprint (AS608) & NFC (PN532) HID Device**

Perangkat biometrik berbasis **Raspberry Pi Pico** yang berfungsi sebagai **USB HID Keyboard**, mengirimkan data **Fingerprint template (HEX)** atau **UID NFC (eKTP)** langsung ke komputer, diakhiri dengan tombol **TAB**.

Cocok untuk:
- Sistem login
- Form otomatis
- Integrasi aplikasi tanpa driver tambahan
- Prototipe sistem identitas

---

## ✨ Fitur Utama

- 🔐 **Fingerprint Scanner (AS608)**
  - Mengambil template fingerprint
  - Trim trailing `0x00` agar data lebih efisien
  - Output HEX via USB HID + TAB

- 🪪 **NFC / eKTP Reader (PN532)**
  - Membaca UID kartu (ISO14443A)
  - Output HEX via USB HID + TAB

- 🖥 **OLED Display (SSD1306 0.96")**
  - Menu navigasi
  - Status real-time

- 🎛 **3 Tombol Navigasi**
  - MODE (pindah menu)
  - SELECT (OK)
  - BACK (kembali ke menu)

- 🔊 **Buzzer**
  - Beep splash screen
  - Beep sukses

- 💻 **USB HID Keyboard**
  - Tidak perlu driver
  - Plug & play di Windows / Linux / macOS

---

## 🧠 Cara Kerja Singkat

1. Device terhubung ke PC via USB
2. Device dikenali sebagai **keyboard**
3. Data biometrik diketik otomatis ke field aktif
4. Diakhiri dengan tombol **TAB**

---

## 🧩 Hardware yang Digunakan

| Komponen | Keterangan |
|--------|-----------|
| MCU | Raspberry Pi Pico |
| Fingerprint | AS608 (UART) |
| NFC | PN532 (SPI Mode) |
| OLED | SSD1306 128x64 I2C |
| Tombol | Tactile switch (INPUT_PULLUP) |
| Buzzer | Active buzzer |
| Tegangan | 5V & 3.3V (common GND) |

---

## 📌 Pin Mapping

### OLED (I2C)
| Fungsi | GPIO |
|------|------|
| SDA | GPIO 4 |
| SCL | GPIO 5 |

### PN532 (SPI)
| Fungsi | GPIO |
|------|------|
| SCK | GPIO 18 |
| MOSI | GPIO 19 |
| MISO | GPIO 16 |
| SS | GPIO 17 |

### Fingerprint AS608 (UART)
| Fungsi | GPIO |
|------|------|
| TX Pico → RX AS608 | GPIO 0 |
| RX Pico ← TX AS608 | GPIO 1 |

### Tombol
| Tombol | GPIO |
|------|------|
| MODE | GPIO 10 |
| SELECT | GPIO 11 |
| BACK | GPIO 12 |

### Buzzer
| Fungsi | GPIO |
|------|------|
| Buzzer + | GPIO 15 |
| Buzzer − | GND |

---

## ⚡ Power & Kapasitor (PENTING)

Semua jalur daya digabung di PCB:
- **5V (VSYS)**
- **3V3**
- **GND (common)**

### Rekomendasi Kapasitor (final)

#### Bulk (stabilisasi utama)
- **1000µF – 2200µF elektrolit**
  - 5V ↔ GND (dekat Pico / VSYS)
  - 3V3 ↔ GND (dekat Pico)

#### Decoupling (wajib)
- **100nF keramik**
  - Di setiap modul:
    - PN532
    - AS608
    - OLED
  - Sedekat mungkin dengan pin VCC–GND

#### Tambahan (disarankan)
- **10µF – 47µF**
  - Paralel dengan 100nF di setiap modul

✅ Kapasitor boleh **diparalel**  
✅ Boleh dipasang **di PCB + langsung di modul**

---

## 🎛 Navigasi Menu

- **MODE** : pindah pilihan menu
- **SELECT** : masuk menu
- **BACK** : kembali ke menu utama

### Menu:
1. Fingerprint
2. eKTP / NFC

---

## 🧪 Output Contoh

### Fingerprint (HEX)
EF01AABBCCDD...


### NFC UID (HEX)
04A1B2C3D4


(Setelah data → otomatis tekan **TAB**)

---

## 📦 Library yang Digunakan

- Adafruit SSD1306
- Adafruit GFX
- Adafruit PN532
- Adafruit Fingerprint Sensor Library
- Keyboard (TinyUSB / Pico core)

---

## 🚨 Catatan Penting

- Project ini **TIDAK melakukan matching biometrik**
- Hanya **capture & forward data**
- Keamanan & verifikasi dilakukan di **host / aplikasi**

---

## 📜 License

MIT License

Copyright (c) 2026 SUPANGAT

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction.

---

## 👤 Author

**SUPANGAT**  
Biometric & Embedded Systems Project  
Indonesia 🇮🇩
