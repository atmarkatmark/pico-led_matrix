// #include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "pins.h"
#include "pix.h"
#include "chars.h"

const char *ssid = SSID;
const char *psk = PSK;

uint8_t d[ROWS][COLS];
uint8_t ds[10][ROWS][COLS];

void init_display() {
  for (int r = 0; r != ROWS; ++r) {
    for (int c = 0; c != COLS; ++c) {
      uint8_t br = r << 4;
      switch (c % 8) {
        case 0: d[r][c] = RED;  break;
        case 1: d[r][c] = GREEN;  break;
        case 2: d[r][c] = BLUE;  break;
        case 3: d[r][c] = YELLOW;  break;
        case 4: d[r][c] = MEGENTA;  break;
        case 5: d[r][c] = CYAN;  break;
        case 6: d[r][c] = WHITE;  break;
        case 7: d[r][c] = BLACK;  break;
      }
      d[r][c] |= br;
    }
  }
}

void put_char(uint8_t d[ROWS][COLS], uint8_t dx, uint8_t dy, const uint8_t c[8][8], size_t size) {
  for (int y = 0; y != size; ++y) {
    for (int x = 0; x != size; ++x) {
      d[dy + y][dx + x] = c[y][x] | 0xf0;
    }
  }
}

// Core 0
void setup()
{
  // Initialize HUB75
  for (int i = 0; i <= 13; ++i)
    pinMode(i, OUTPUT);
  
  // Set all pin low but OE(Output Enable) high(active low)
  for (int i = 0; i != 13; ++i)
    digitalWrite(i, LOW);
  digitalWrite(OE, HIGH);

  Serial.begin(9600);
  Serial.print("Start");
  // delay(3000);

  init_display();
  put_char(d, 0, 0, N0, 8);
  put_char(d, 8, 0, N1, 8);
  put_char(d, 16, 0, N2, 8);
  put_char(d, 24, 0, N3, 8);
  put_char(d, 32, 0, N4, 8);
  put_char(d, 40, 0, N5, 8);
  put_char(d, 48, 0, N6, 8);
  put_char(d, 56, 0, N7, 8);

  uint8_t colors[] = { RED, RED | GREEN, GREEN, GREEN | BLUE, BLUE | RED, WHITE, WHITE };
  for (int i = 0; i != sizeof(colors); ++i)
    memset(ds[i], colors[i], ROWS * COLS);
}

// Core 1
void setup1() {
  delay(1000);

  Serial.println("Core1: Connecting to AP...");
  WiFi.begin(ssid, psk);
  Serial.println("Core1: Successfully connected to AP");
  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com");
  // https://arduino-pico.readthedocs.io/en/latest/wifintp.html
  // NTP.begin("ntp.nict.jp", "time.google.com");
  // NTP.waitSet();
  time_t now = time(NULL);
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

// Core 1
void loop1() {
  // Serial.println("Core1: loop");

  delay(5000);

  // rp2040.idleOtherCore();
  // memcpy(d, d_green, COLS);
  // rp2040.resumeOtherCore();
}

// Core 0
uint32_t count = 0; // 10ms秒までのus積算時間
uint32_t interval = 1000000;  // 1ループかかる時間
uint8_t frame = 0;
void loop()
{
  unsigned long start = micros();

  // 10ms単位で明るさ制御
  if (10000 < (count += interval)) {
    count = 0;
  }

  for (int r = 0; r != 16; ++r) {
    for (int c = 0; c != COLS; ++c) {
      // Set color
      uint8_t p0 = d[r][c];
      uint8_t p1 = d[r + 16][c];
      // uint8_t p0 = ds[frame][r][c];
      // uint8_t p1 = ds[frame][r][c];
      uint32_t br0 = ((p0 & 0xf0) >> 4) * MAX_BRIGHTNESS_RATE * 100; // 明るさだけ抽出
      uint32_t br1 = ((p1 & 0xf0) >> 4) * MAX_BRIGHTNESS_RATE * 100;
      p0 &= 0x0f; // 明るさビットを除去して色情報だけ残す
      p1 &= 0x0f;

      // Serial.printf("%d %d\n", br0, br1);
      // delay(50);
      // Brightness(darker 1000 << 10000 brighter)
      if (br0 < count)
        p0 = BLACK;
      if (br1 < count)
        p1 = BLACK;

      gpio_put_masked(RGB0_MASK, p0 << R0);
      gpio_put_masked(RGB1_MASK, p1 << R1);
      
      // Clock to send RGB value
      gpio_put(CLK, HIGH);
      asm("nop \n"); // 入れないと速すぎてダメっぽい
      gpio_put(CLK, LOW);
    }


    gpio_put_masked(ADDR_MASK, r << ADDR_A);

    gpio_put(LAT, HIGH);
    gpio_put(LAT, LOW);
    digitalWrite(OE, HIGH); // ここだけgpio_put()だとnop()を入れても表示が崩れる
    digitalWrite(OE, LOW);
  }

  interval = micros() - start;
}
