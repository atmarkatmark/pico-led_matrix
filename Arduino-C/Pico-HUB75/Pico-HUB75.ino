// #include <Arduino.h>
#include <WiFi.h>

#include "config.h"

const char *ssid = SSID;
const char *psk = PSK;

WiFiMulti wifiMulti;

#define ROWS 32
#define COLS 64

#define R0 0
#define G0 1
#define B0 2
#define RGB0_MASK (1 << R0) | (1 << G0) | (1 << B0)
#define R1 3
#define G1 4
#define B1 5
#define RGB1_MASK (1 << R1) | (1 << G1) | (1 << B1)
#define ADDR_A 6
#define ADDR_B 7
#define ADDR_C 8
#define ADDR_D 9
#define ADDR_E 10
#define ADDR_MASK (1 << ADDR_A) | (1 << ADDR_B) | (1 << ADDR_C) | (1 << ADDR_D)
#define CLK 11
#define LAT 12
#define OE 13

#define RED 1
#define GREEN 1 << 1
#define BLUE 1 << 2
#define YELLOW RED | GREEN
#define MEGENTA RED | BLUE
#define CYAN BLUE | GREEN
#define WHITE RED | GREEN | BLUE
#define BLACK 0

uint8_t d[ROWS][COLS];
uint8_t d_red[ROWS][COLS] = { RED };
uint8_t d_green[ROWS][COLS] = { GREEN };
uint8_t d_blue[ROWS][COLS] = { BLUE };

void init_display() {
  for (int r = 0; r != ROWS; ++r) {
    for (int c = 0; c != COLS; ++c) {
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
    }
  }
}

void setup()
{
  // Initialize HUB75
  for (int i = 0; i <= 13; ++i)
    pinMode(i, OUTPUT);
  for (int i = 0; i != 13; ++i)
    digitalWrite(i, LOW);
  digitalWrite(OE, HIGH);

  Serial.begin(9600);
  Serial.print("Start");

  init_display();
}

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

void loop1() {
  // Serial.println("Core1: loop");

  delay(5000);

  // rp2040.idleOtherCore();
  // memcpy(d, d_green, COLS);
  // rp2040.resumeOtherCore();
}

uint32_t count = 0; // 積算時間
uint32_t interval = 1000000;  // 1ループかかる時間
uint8_t brightness = 100; // 明るさを 暗0-100明 で
uint8_t step = -1;
void loop()
{
  unsigned long start = micros();

  if (10000 < (count += interval)) {
    count = 0;
    brightness += step;
    if (brightness == 0 || brightness == 100)
      step *= -1;
  }

  for (int r = 0; r != 16; ++r) {
    for (int c = 0; c != COLS; ++c) {
      // Set color
      uint8_t p0 = d[r][c];
      uint8_t p1 = d[r][c];

      // Brightness(darker 1000 << 10000 brighter)
      // if (1000 < count) {
      if (brightness * 100 < count) {
        p0 = p1 = BLACK;
      }

      // digitalWrite(R0, p0 & RED);
      // digitalWrite(G0, p0 & GREEN);
      // digitalWrite(B0, p0 & BLUE);
      // digitalWrite(R1, p1 & RED);
      // digitalWrite(G1, p1 & GREEN);
      // digitalWrite(B1, p1 & BLUE);
      // gpio_put(R0, p0 & RED);
      // gpio_put(G0, p0 & GREEN);
      // gpio_put(B0, p0 & BLUE);
      gpio_put_masked(RGB0_MASK, p0 << R0);
      // gpio_put(R1, p1 & RED);
      // gpio_put(G1, p1 & GREEN);
      // gpio_put(B1, p1 & BLUE);
      gpio_put_masked(RGB1_MASK, p1 << R1);
      
      // Clock to send
      // digitalWrite(CLK, HIGH);
      // digitalWrite(CLK, LOW);
      gpio_put(CLK, HIGH);
      // delayMicroseconds(0); // 入れないと速すぎてダメっぽい
      asm("nop \n");
      gpio_put(CLK, LOW);
    }


    // digitalWrite(ADDR_A, r & 1);
    // digitalWrite(ADDR_B, (r >> 1) & 1);
    // digitalWrite(ADDR_C, (r >> 2) & 1);
    // digitalWrite(ADDR_D, (r >> 3) & 1);
    // gpio_put(ADDR_A, r & 1);
    // gpio_put(ADDR_B, (r >> 1) & 1);
    // gpio_put(ADDR_C, (r >> 2) & 1);
    // gpio_put(ADDR_D, (r >> 3) & 1);
    gpio_put_masked(ADDR_MASK, r << ADDR_A);

    // digitalWrite(LAT, HIGH);
    // digitalWrite(LAT, LOW);
    gpio_put(LAT, HIGH);
    gpio_put(LAT, LOW);
    digitalWrite(OE, HIGH); // ここだけgpio_put()だとnop()を入れても表示が崩れる
    digitalWrite(OE, LOW);
    // gpio_put(OE, HIGH);
    // gpio_put(OE, LOW);
  }

  interval = micros() - start;
  // Serial.printf("%d %d\n", interval, count);
}
