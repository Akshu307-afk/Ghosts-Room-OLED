/*
  Ghost's Room - ESP32 OLED companion
  Hardware: ESP32 + 128x64 SSD1306 I2C OLED
  Wiring: SDA=21, SCL=22, OLED address=0x3C

  Libraries:
    - Adafruit GFX Library
    - Adafruit SSD1306

  Keep your Wi-Fi credentials private. Enter them locally before uploading;
  do not commit real credentials to a public repository.
*/

#include <WiFi.h>
#include <Wire.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Wi-Fi: fill these in on your own computer ----------
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ---------- Display and timezone ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SDA_PIN 21
#define SCL_PIN 22

const long GMT_OFFSET_SECONDS = 19800;  // India: UTC+05:30
const int DAYLIGHT_OFFSET_SECONDS = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long lastFrame = 0;
unsigned long lastWiFiAttempt = 0;
const unsigned long FRAME_INTERVAL = 90;
const unsigned long WIFI_RETRY_INTERVAL = 15000;

bool displayReady = false;

// Small 16x16 ghost sprite. Drawn in white, with eyes drawn separately.
const unsigned char PROGMEM ghostBitmap[] = {
  0x03, 0xC0,
  0x0F, 0xF0,
  0x1F, 0xF8,
  0x3F, 0xFC,
  0x3F, 0xFC,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x7F, 0xFE,
  0x6F, 0xF6,
  0x47, 0xE2,
  0x03, 0xC0,
  0x00, 0x00
};

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  if (strcmp(WIFI_SSID, "YOUR_WIFI_NAME") == 0) return;

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWiFiAttempt = millis();
}

void maintainWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  if (millis() - lastWiFiAttempt >= WIFI_RETRY_INTERVAL) {
    connectWiFi();
  }
}

void drawStars(unsigned long now) {
  // Fixed star positions with gentle twinkle.
  const int starX[] = {5, 18, 31, 55, 76, 91, 111, 122, 68, 9, 102};
  const int starY[] = {7, 17, 4, 13, 5, 19, 8, 27, 23, 34, 36};

  for (int i = 0; i < 11; i++) {
    bool bright = ((now / 350 + i * 3) % 5) != 0;
    if (bright) {
      display.drawPixel(starX[i], starY[i], SSD1306_WHITE);
    } else {
      display.drawFastHLine(starX[i] - 1, starY[i], 2, SSD1306_WHITE);
    }
  }
}

void drawMoon() {
  display.fillCircle(108, 13, 8, SSD1306_WHITE);
  display.fillCircle(112, 10, 8, SSD1306_BLACK);
  display.drawPixel(100, 4, SSD1306_WHITE);
}

void drawGhost(int x, int y, bool blink) {
  display.drawBitmap(x, y, ghostBitmap, 16, 16, SSD1306_WHITE);

  // Eyes sit inside the sprite. Blink by briefly drawing a short line.
  if (blink) {
    display.drawFastHLine(x + 4, y + 7, 3, SSD1306_BLACK);
    display.drawFastHLine(x + 10, y + 7, 3, SSD1306_BLACK);
  } else {
    display.drawPixel(x + 5, y + 7, SSD1306_BLACK);
    display.drawPixel(x + 11, y + 7, SSD1306_BLACK);
    display.drawPixel(x + 5, y + 8, SSD1306_BLACK);
    display.drawPixel(x + 11, y + 8, SSD1306_BLACK);
  }
}

void drawClock() {
  struct tm timeInfo;
  bool timeAvailable = getLocalTime(&timeInfo, 20);

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(7, 39);

  if (timeAvailable) {
    char timeText[9];
    strftime(timeText, sizeof(timeText), "%H:%M", &timeInfo);
    display.print(timeText);
  } else {
    display.print("--:--");
  }

  display.setTextSize(1);
  display.setCursor(79, 43);

  if (timeAvailable) {
    char dateText[12];
    strftime(dateText, sizeof(dateText), "%d %b", &timeInfo);
    display.print(dateText);
  } else {
    display.print("Waiting");
  }
}

void drawStatus() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 57);

  if (WiFi.status() == WL_CONNECTED) {
    display.print("WiFi connected");
    display.drawCircle(119, 59, 3, SSD1306_WHITE);
    display.drawPixel(119, 59, SSD1306_WHITE);
  } else {
    display.print("WiFi offline");
    display.drawCircle(119, 59, 3, SSD1306_WHITE);
  }
}

void drawScene(unsigned long now) {
  display.clearDisplay();
  drawStars(now);
  drawMoon();

  // Smooth bobbing motion.
  int bob = (int)(3.0 * sin(now / 430.0));
  bool blink = (now % 4200) < 150;
  drawGhost(43, 17 + bob, blink);

  // Tiny ground/dream line under the ghost.
  display.drawFastHLine(38, 36 + bob, 26, SSD1306_WHITE);

  drawClock();
  drawStatus();
  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  displayReady = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  if (!displayReady) {
    Serial.println("SSD1306 display not found. Check wiring and I2C address.");
    while (true) delay(100);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(22, 25);
  display.print("Ghost's Room");
  display.setCursor(25, 39);
  display.print("Waking up...");
  display.display();

  connectWiFi();

  // Configure IST time. NTP sync begins when Wi-Fi is available.
  configTime(GMT_OFFSET_SECONDS, DAYLIGHT_OFFSET_SECONDS,
             "pool.ntp.org", "time.google.com");
}

void loop() {
  maintainWiFi();

  unsigned long now = millis();
  if (now - lastFrame >= FRAME_INTERVAL) {
    lastFrame = now;
    drawScene(now);
  }
}
