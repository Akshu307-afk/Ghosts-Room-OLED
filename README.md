# Ghost's Room 👻

A cozy animated pixel-art ghost companion powered by an ESP32 and a 128×64 SSD1306 OLED display. A tiny ghost floats through a quiet night sky while the ESP32 keeps the clock and date synchronized over Wi-Fi.

## Features
- Floating ghost animation with blinking eyes
- Crescent moon and twinkling stars
- Live clock and date using NTP
- Wi-Fi connection indicator
- Indian Standard Time (IST, UTC+05:30)
- No physical buttons required

## Hardware
- ESP32 development board (ESP32 Dev Module)
- 128×64 SSD1306 I²C OLED display
- Jumper wires and USB cable

## Wiring

| OLED pin | ESP32 pin |
|---|---|
| GND | GND |
| VDD / VCC | 3V3 |
| SCK / SCL | GPIO 22 |
| SDA | GPIO 21 |

The sketch uses I²C address `0x3C`. See `docs/wiring.svg` for a wiring reference.

## Arduino IDE setup
1. Install **ESP32 by Espressif Systems** in Boards Manager. If needed, add this URL in Preferences:
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
2. Install **Adafruit GFX Library** and **Adafruit SSD1306** through Library Manager.
3. Open `GhostsRoom.ino`.
4. Enter your Wi-Fi network name and password in the local sketch.
5. Select **ESP32 Dev Module**, choose the correct port, and upload.

> **Security:** Keep real Wi-Fi credentials private. Do not commit your password to this public repository.

## Default configuration
- OLED: 128×64, I²C address `0x3C`
- SDA: GPIO 21
- SCL: GPIO 22
- Time zone: IST (UTC+05:30)
- NTP servers: `pool.ntp.org` and `time.google.com`

The animation can run without Wi-Fi. The clock will display placeholders until time synchronization succeeds.

## Project structure
```
Ghosts-Room-OLED/
├── GhostsRoom.ino
├── README.md
├── LICENSE
└── docs/
    └── wiring.svg
```

## Project goal
A little pixel-art personality for a bedroom desk: calm night-time visuals, a friendly ghost, and a useful clock in one ESP32 project.

## License
MIT License. See [LICENSE](LICENSE).
