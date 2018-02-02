#ifndef FONT4x6x1_C
#define FONT4x6x1_C

#include <stdint.h>
extern uint8_t FONT4x6x1[];

#if __INCLUDE_LEVEL__ == 0 || defined(PIXIE_NOLIB)

uint8_t FONT4x6x1[] = {
0x00,0x00,0x00,0x00,0x00,0x00, // null
0xF0,0xF0,0xF0,0xF0,0xF0,0xF0, // BLOCK
0x60,0x60,0x60,0x60,0x60,0x60, // vertical bar
0x00,0x00,0xFF,0xFF,0x00,0x00, // horizontal bar
0x60,0x60,0xFF,0xFF,0x60,0x60, // cross bar
0x60,0x60,0xEE,0xEE,0x00,0x00, // bottom-right corner
0x60,0x60,0x70,0x70,0x00,0x00, // bottom-left corner
0x00,0x00,0x70,0x70,0x60,0x60, // top-left corner
0x00,0x00,0xEE,0xEE,0x60,0x60, // top-right corner
0xF0,0x90,0x90,0x90,0xF0,0xF0, 
0xF0,0x90,0x90,0xF0,0x90,0xF0, 
0xF0,0x90,0x90,0xF0,0xF0,0xF0, 
0xF0,0x90,0xF0,0x90,0x90,0xF0, 
0xF0,0x90,0xF0,0x90,0xF0,0xF0, 
0xF0,0x90,0xF0,0xF0,0x90,0xF0, 
0xF0,0x90,0xF0,0xF0,0xF0,0xF0, 
0xF0,0xF0,0x90,0x90,0x90,0xF0, 
0xF0,0xF0,0x90,0x90,0xF0,0xF0, 
0xF0,0xF0,0x90,0xF0,0x90,0xF0, 
0xF0,0xF0,0x90,0xF0,0xF0,0xF0, 
0xF0,0xF0,0xF0,0x90,0x90,0xF0, 
0xF0,0xF0,0xF0,0x90,0xF0,0xF0, 
0xF0,0xF0,0xF0,0xF0,0x90,0xF0, 
0xF0,0xF0,0xF0,0xF0,0xF0,0xF0, 
0xF0,0x90,0x90,0x90,0x90,0xF0, 
0xF0,0x90,0x90,0x90,0xF0,0xF0, 
0xF0,0x90,0x90,0xF0,0x90,0xF0, 
0xF0,0x90,0x90,0xF0,0xF0,0xF0, 
0xF0,0x90,0xF0,0x90,0x90,0xF0, 
0xF0,0x90,0xF0,0x90,0xF0,0xF0, 
0xF0,0x90,0xF0,0xF0,0x90,0xF0, 
0xF0,0x90,0xF0,0xF0,0xF0,0xF0, 
0x00,0x00,0x00,0x00,0x00,0x00, // space
0x40,0x40,0x40,0x00,0x40,0x00, // !
0xA0,0xA0,0x00,0x00,0x00,0x00, // "
0xA0,0xE0,0xA0,0xE0,0xA0,0x00, // #
0x60,0xC0,0x60,0xC0,0x40,0x00, // $
0x80,0x20,0x40,0x80,0x20,0x00, // %
0x40,0xA0,0x40,0xA0,0x60,0x00, // &
0x40,0x40,0x00,0x00,0x00,0x00, // '
0x20,0x40,0x40,0x40,0x20,0x00, // (
0x80,0x40,0x40,0x40,0x80,0x00, // )
0xA0,0x40,0xE0,0x40,0xA0,0x00, // *
0x00,0x40,0xE0,0x40,0x00,0x00, // +
0x00,0x00,0x00,0x40,0x80,0x00, // ,
0x00,0x00,0xE0,0x00,0x00,0x00, // -
0x00,0x00,0x00,0x40,0x40,0x00, // .
0x00,0x20,0x40,0x80,0x00,0x00, // /
0xE0,0xA0,0xA0,0xA0,0xE0,0x00, // 0
0x40,0x40,0x40,0x40,0x40,0x00, // 1
0xE0,0x20,0xE0,0x80,0xE0,0x00, // 2
0xE0,0x20,0x60,0x20,0xE0,0x00, // 3
0xA0,0xA0,0xE0,0x20,0x20,0x00, // 4
0xE0,0x80,0xE0,0x20,0xE0,0x00, // 5
0xE0,0x80,0xE0,0xA0,0xE0,0x00, // 6
0xE0,0x20,0x20,0x20,0x20,0x00, // 7
0xE0,0xA0,0xE0,0xA0,0xE0,0x00, // 8
0xE0,0xA0,0xE0,0x20,0xE0,0x00, // 9
0x00,0x40,0x00,0x40,0x00,0x00, // :
0x00,0x40,0x00,0x40,0x80,0x00, // ;
0x20,0x40,0x80,0x40,0x20,0x00, // <
0x00,0xE0,0x00,0xE0,0x00,0x00, // =
0x80,0x40,0x20,0x40,0x80,0x00, // >
0xE0,0x20,0x40,0x00,0x40,0x00, // ?
0x40,0xA0,0xE0,0x80,0x60,0x00, // @
0x40,0xA0,0xE0,0xA0,0xA0,0x00, // A
0xC0,0xA0,0xC0,0xA0,0xC0,0x00, // B
0x60,0x80,0x80,0x80,0x60,0x00, // C
0xC0,0xA0,0xA0,0xA0,0xC0,0x00, // D
0xE0,0x80,0xE0,0x80,0xE0,0x00, // E
0xE0,0x80,0xE0,0x80,0x80,0x00, // F
0x60,0x80,0xE0,0xA0,0x60,0x00, // G
0xA0,0xA0,0xE0,0xA0,0xA0,0x00, // H
0xE0,0x40,0x40,0x40,0xE0,0x00, // I
0x20,0x20,0x20,0xA0,0x40,0x00, // J
0xA0,0xA0,0xC0,0xA0,0xA0,0x00, // K
0x80,0x80,0x80,0x80,0xE0,0x00, // L
0xA0,0xE0,0xE0,0xA0,0xA0,0x00, // M
0xA0,0xE0,0xE0,0xE0,0xA0,0x00, // N
0x40,0xA0,0xA0,0xA0,0x40,0x00, // O
0xC0,0xA0,0xC0,0x80,0x80,0x00, // P
0x40,0xA0,0xA0,0xE0,0x60,0x00, // Q
0xC0,0xA0,0xE0,0xC0,0xA0,0x00, // R
0x60,0x80,0x40,0x20,0xC0,0x00, // S
0xE0,0x40,0x40,0x40,0x40,0x00, // T
0xA0,0xA0,0xA0,0xA0,0x60,0x00, // U
0xA0,0xA0,0xA0,0x40,0x40,0x00, // V
0xA0,0xA0,0xE0,0xE0,0xA0,0x00, // W
0xA0,0xA0,0x40,0xA0,0xA0,0x00, // X
0xA0,0xA0,0x40,0x40,0x40,0x00, // Y
0xE0,0x20,0x40,0x80,0xE0,0x00, // Z
0xE0,0x80,0x80,0x80,0xE0,0x00, // [
0x00,0x80,0x40,0x20,0x00,0x00, // backslash
0xE0,0x20,0x20,0x20,0xE0,0x00, // ]
0x40,0xA0,0x00,0x00,0x00,0x00, // ^
0x00,0x00,0x00,0x00,0xE0,0x00, // _
0x80,0x40,0x00,0x00,0x00,0x00, // `
0x00,0x00,0x60,0xA0,0xE0,0x00, // a
0x80,0x80,0xC0,0xA0,0xC0,0x00, // b
0x00,0x00,0x60,0x80,0x60,0x00, // c
0x20,0x20,0x60,0xA0,0x60,0x00, // d
0x00,0x60,0xA0,0xC0,0x60,0x00, // e
0x20,0x40,0xE0,0x40,0x40,0x00, // f
0x00,0x60,0xE0,0x20,0xC0,0x00, // g
0x80,0x80,0xC0,0xA0,0xA0,0x00, // h
0x40,0x00,0x40,0x40,0x40,0x00, // i
0x20,0x00,0x20,0x20,0xC0,0x00, // j
0x00,0x80,0xA0,0xC0,0xA0,0x00, // k
0xC0,0x40,0x40,0x40,0xE0,0x00, // l
0x00,0x00,0xE0,0xE0,0xA0,0x00, // m
0x00,0x00,0xC0,0xA0,0xA0,0x00, // n
0x00,0x00,0x40,0xA0,0x40,0x00, // o
0x00,0x40,0xA0,0xC0,0x80,0x00, // p
0x00,0x40,0xA0,0x60,0x20,0x00, // q
0x00,0x80,0xE0,0x80,0x80,0x00, // r
0x00,0x60,0xC0,0x60,0xC0,0x00, // s
0x40,0xE0,0x40,0x40,0x40,0x00, // t
0x00,0x00,0xA0,0xA0,0x60,0x00, // u
0x00,0x00,0xA0,0xE0,0x40,0x00, // v
0x00,0x00,0xA0,0xE0,0xE0,0x00, // w
0x00,0x00,0xA0,0x40,0xA0,0x00, // x
0x00,0xA0,0x60,0x20,0xC0,0x00, // y
0x00,0xE0,0x60,0xC0,0xE0,0x00, // z
0x60,0x40,0x80,0x40,0x60,0x00, // {
0x40,0x40,0x00,0x40,0x40,0x00, // |
0xC0,0x40,0x20,0x40,0xC0,0x00, // }
0x60,0xC0,0x00,0x00,0x00,0x00, // ~
0x00,0x00,0x00,0x00,0x00,0x00,
};

#endif
#endif