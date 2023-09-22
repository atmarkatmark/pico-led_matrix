// #include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "pins.h"
#include "pix.h"

const char *ssid = SSID;
const char *psk = PSK;
WiFiMulti wifiMulti;

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
  delay(3000);

  init_display();

  uint8_t colors[] = { RED, RED | GREEN, GREEN, GREEN | BLUE, BLUE | RED, WHITE, WHITE };
  for (int i = 0; i != sizeof(colors); ++i)
    memset(ds[i], colors[i], ROWS * COLS);
}

// Core 1
void setup1() {
  delay(1000);
  wifiMulti.addAP(ssid, psk);
  Serial.println("Core1: setup");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("Core1: Successfully connected to AP");
    configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com");
  } else {
    Serial.println("Core1: Failed to connect to AP");
  }
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
      uint8_t p1 = d[r][c];
      // uint8_t p0 = ds[frame][r][c];
      // uint8_t p1 = ds[frame][r][c];
      uint16_t br0 = ((p0 & 0xf0) >> 4) * 6 * 100; // 明るさだけ抽出
      uint16_t br1 = ((p1 & 0xf0) >> 4) * 6 * 100;
      p0 &= 0x0f; // 明るさビットを除去して色情報だけ残す
      p1 &= 0x0f;

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
