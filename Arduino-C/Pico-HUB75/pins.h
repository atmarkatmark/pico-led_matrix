#ifndef __PINS_H__
#define __PINS_H__

// HUB75ピン番号設定
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
#define ADDR_E 10 // 64x32 doesn't use this pin
#define ADDR_MASK (1 << ADDR_A) | (1 << ADDR_B) | (1 << ADDR_C) | (1 << ADDR_D)
#define CLK 11
#define LAT 12
#define OE 13

#endif  // __PINS_H__