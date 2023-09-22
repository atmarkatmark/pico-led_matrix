#ifndef __PIX_H__
#define __PIX_H__

// 色定義
/*
 * uint8_tの上位4bitを明るさ、下位4bitを色指定にする
 * 例えば0b11110001は明るさ1111(最大)の赤、0b10000010は明るさ1000の緑
 * 明るさは百分率にすると100/16 = 6.25となるので、* 6として計算する
 */
#define RED 1
#define GREEN 1 << 1
#define BLUE 1 << 2
#define YELLOW RED | GREEN
#define MEGENTA RED | BLUE
#define CYAN BLUE | GREEN
#define WHITE RED | GREEN | BLUE
#define BLACK 0

#endif  // __PIX_H__
