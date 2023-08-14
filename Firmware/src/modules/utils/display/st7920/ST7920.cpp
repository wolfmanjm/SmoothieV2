#include "ST7920.h"
#include "ConfigReader.h"
#include "main.h"
#include "Pin.h"
#include "benchmark_timer.h"

#include <cstring>

// memory mapped character glyphs
static const uint8_t font5x8[] = {
    // 5x8 font each byte is consecutive x bits left aligned then each subsequent byte is Y 8 bytes per character
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xF8, 0xA8, 0xF8, 0xD8, 0x88, 0x70, 0x00, // 0x00, 0x01
    0x70, 0xF8, 0xA8, 0xF8, 0x88, 0xD8, 0x70, 0x00, 0x00, 0x50, 0xF8, 0xF8, 0xF8, 0x70, 0x20, 0x00, // 0x02, 0x03
    0x00, 0x20, 0x70, 0xF8, 0xF8, 0x70, 0x20, 0x00, 0x70, 0x50, 0xF8, 0xA8, 0xF8, 0x20, 0x70, 0x00, // 0x04, 0x05
    0x20, 0x70, 0xF8, 0xF8, 0xF8, 0x20, 0x70, 0x00, 0x00, 0x00, 0x20, 0x70, 0x70, 0x20, 0x00, 0x00, // 0x06, 0x07
    0xF8, 0xF8, 0xD8, 0x88, 0x88, 0xD8, 0xF8, 0xF8, 0x00, 0x00, 0x20, 0x50, 0x50, 0x20, 0x00, 0x00, // 0x08, 0x09
    0xF8, 0xF8, 0xD8, 0xA8, 0xA8, 0xD8, 0xF8, 0xF8, 0x00, 0x38, 0x18, 0x68, 0xA0, 0xA0, 0x40, 0x00, // 0x0A, 0x0B
    0x70, 0x88, 0x88, 0x70, 0x20, 0xF8, 0x20, 0x00, 0x78, 0x48, 0x78, 0x40, 0x40, 0x40, 0xC0, 0x00, // 0x0C, 0x0D
    0x78, 0x48, 0x78, 0x48, 0x48, 0x58, 0xC0, 0x00, 0x20, 0xA8, 0x70, 0xD8, 0xD8, 0x70, 0xA8, 0x20, // 0x0E, 0x0F
    0x80, 0xC0, 0xF0, 0xF8, 0xF0, 0xC0, 0x80, 0x00, 0x08, 0x18, 0x78, 0xF8, 0x78, 0x18, 0x08, 0x00, // 0x10, 0x11
    0x20, 0x70, 0xA8, 0x20, 0xA8, 0x70, 0x20, 0x00, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0x00, 0xD8, 0x00, // 0x12, 0x13
    0x78, 0xA8, 0xA8, 0x68, 0x28, 0x28, 0x28, 0x00, 0x30, 0x48, 0x50, 0x28, 0x10, 0x48, 0x48, 0x30, // 0x14, 0x15
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0x00, 0x20, 0x70, 0xA8, 0x20, 0xA8, 0x70, 0x20, 0xF8, // 0x16, 0x17
    0x00, 0x20, 0x70, 0xA8, 0x20, 0x20, 0x20, 0x00, 0x00, 0x20, 0x20, 0x20, 0xA8, 0x70, 0x20, 0x00, // 0x18, 0x19
    0x00, 0x20, 0x10, 0xF8, 0x10, 0x20, 0x00, 0x00, 0x00, 0x20, 0x40, 0xF8, 0x40, 0x20, 0x00, 0x00, // 0x1A, 0x1B
    0x00, 0x80, 0x80, 0x80, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x50, 0xF8, 0xF8, 0x50, 0x00, 0x00, 0x00, // 0x1C, 0x1D
    0x00, 0x20, 0x20, 0x70, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0x70, 0x20, 0x20, 0x00, 0x00, // 0x1E, 0x1F
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00, // 0x20, 0x21
    0x50, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x50, 0x00, // 0x22, 0x23
    0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00, 0xC0, 0xC8, 0x10, 0x20, 0x40, 0x98, 0x18, 0x00, // 0x24, 0x25
    0x40, 0xA0, 0xA0, 0x40, 0xA8, 0x90, 0x68, 0x00, 0x30, 0x30, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, // 0x26, 0x27
    0x10, 0x20, 0x40, 0x40, 0x40, 0x20, 0x10, 0x00, 0x40, 0x20, 0x10, 0x10, 0x10, 0x20, 0x40, 0x00, // 0x28, 0x29
    0x20, 0xA8, 0x70, 0xF8, 0x70, 0xA8, 0x20, 0x00, 0x00, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00, 0x00, // 0x2A, 0x2B
    0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x20, 0x40, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, // 0x2C, 0x2D
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, // 0x2E, 0x2F
    0x70, 0x88, 0x98, 0xA8, 0xC8, 0x88, 0x70, 0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, // 0x30, 0x31
    0x70, 0x88, 0x08, 0x70, 0x80, 0x80, 0xF8, 0x00, 0xF8, 0x08, 0x10, 0x30, 0x08, 0x88, 0x70, 0x00, // 0x32, 0x33
    0x10, 0x30, 0x50, 0x90, 0xF8, 0x10, 0x10, 0x00, 0xF8, 0x80, 0xF0, 0x08, 0x08, 0x88, 0x70, 0x00, // 0x34, 0x35
    0x38, 0x40, 0x80, 0xF0, 0x88, 0x88, 0x70, 0x00, 0xF8, 0x08, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, // 0x36, 0x37
    0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00, 0x70, 0x88, 0x88, 0x78, 0x08, 0x10, 0xE0, 0x00, // 0x38, 0x39
    0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x20, 0x40, 0x00, // 0x3A, 0x3B
    0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, // 0x3C, 0x3D
    0x40, 0x20, 0x10, 0x08, 0x10, 0x20, 0x40, 0x00, 0x70, 0x88, 0x08, 0x30, 0x20, 0x00, 0x20, 0x00, // 0x3E, 0x3F
    0x70, 0x88, 0xA8, 0xB8, 0xB0, 0x80, 0x78, 0x00, 0x20, 0x50, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00, // 0x40, 0x41
    0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00, 0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00, // 0x42, 0x43
    0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF8, 0x00, // 0x44, 0x45
    0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x00, 0x78, 0x88, 0x80, 0x80, 0x98, 0x88, 0x78, 0x00, // 0x46, 0x47
    0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00, 0x70, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, // 0x48, 0x49
    0x38, 0x10, 0x10, 0x10, 0x10, 0x90, 0x60, 0x00, 0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00, // 0x4A, 0x4B
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x00, 0x88, 0xD8, 0xA8, 0xA8, 0xA8, 0x88, 0x88, 0x00, // 0x4C, 0x4D
    0x88, 0x88, 0xC8, 0xA8, 0x98, 0x88, 0x88, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, // 0x4E, 0x4F
    0xF0, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x80, 0x00, 0x70, 0x88, 0x88, 0x88, 0xA8, 0x90, 0x68, 0x00, // 0x50, 0x51
    0xF0, 0x88, 0x88, 0xF0, 0xA0, 0x90, 0x88, 0x00, 0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00, // 0x52, 0x53
    0xF8, 0xA8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, // 0x54, 0x55
    0x88, 0x88, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 0x88, 0x88, 0x88, 0xA8, 0xA8, 0xA8, 0x50, 0x00, // 0x56, 0x57
    0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00, 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x20, 0x00, // 0x58, 0x59
    0xF8, 0x08, 0x10, 0x70, 0x40, 0x80, 0xF8, 0x00, 0x78, 0x40, 0x40, 0x40, 0x40, 0x40, 0x78, 0x00, // 0x5A, 0x5B
    0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x78, 0x08, 0x08, 0x08, 0x08, 0x08, 0x78, 0x00, // 0x5C, 0x5D
    0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, // 0x5E, 0x5F
    0x60, 0x60, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x70, 0x90, 0x78, 0x00, // 0x60, 0x61
    0x80, 0x80, 0xB0, 0xC8, 0x88, 0xC8, 0xB0, 0x00, 0x00, 0x00, 0x70, 0x88, 0x80, 0x88, 0x70, 0x00, // 0x62, 0x63
    0x08, 0x08, 0x68, 0x98, 0x88, 0x98, 0x68, 0x00, 0x00, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x70, 0x00, // 0x64, 0x65
    0x10, 0x28, 0x20, 0x70, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x70, 0x98, 0x98, 0x68, 0x08, 0x70, // 0x66, 0x67
    0x80, 0x80, 0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00, 0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00, // 0x68, 0x69
    0x10, 0x00, 0x10, 0x10, 0x10, 0x90, 0x60, 0x00, 0x80, 0x80, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x00, // 0x6A, 0x6B
    0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, 0x00, 0x00, 0xD0, 0xA8, 0xA8, 0xA8, 0xA8, 0x00, // 0x6C, 0x6D
    0x00, 0x00, 0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, // 0x6E, 0x6F
    0x00, 0x00, 0xB0, 0xC8, 0xC8, 0xB0, 0x80, 0x80, 0x00, 0x00, 0x68, 0x98, 0x98, 0x68, 0x08, 0x08, // 0x70, 0x71
    0x00, 0x00, 0xB0, 0xC8, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x78, 0x80, 0x70, 0x08, 0xF0, 0x00, // 0x72, 0x73
    0x20, 0x20, 0xF8, 0x20, 0x20, 0x28, 0x10, 0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0x98, 0x68, 0x00, // 0x74, 0x75
    0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 0x00, 0x00, 0x88, 0x88, 0xA8, 0xA8, 0x50, 0x00, // 0x76, 0x77
    0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x88, 0x88, 0x78, 0x08, 0x88, 0x70, // 0x78, 0x79
    0x00, 0x00, 0xF8, 0x10, 0x20, 0x40, 0xF8, 0x00, 0x10, 0x20, 0x20, 0x40, 0x20, 0x20, 0x10, 0x00, // 0x7A, 0x7B
    0x20, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x00, 0x40, 0x20, 0x20, 0x10, 0x20, 0x20, 0x40, 0x00, // 0x7C, 0x7D
    0x80, 0xC0, 0xF0, 0xF8, 0xF0, 0xC0, 0x80, 0x00, 0x20, 0x70, 0xD8, 0x88, 0x88, 0xF8, 0x00, 0x00, // 0x7E, 0x7F // 7e ~ = 0x40,0xA8,0x10,0x00,0x00,0x00,0x00,0x00
    0x70, 0x88, 0x80, 0x80, 0x88, 0x70, 0x10, 0x60, 0x00, 0x88, 0x00, 0x88, 0x88, 0x98, 0x68, 0x00, // 0x80, 0x81
    0x18, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x78, 0x00, 0xF8, 0x00, 0x60, 0x10, 0x70, 0x90, 0x78, 0x00, // 0x82, 0x83
    0x88, 0x00, 0x60, 0x10, 0x70, 0x90, 0x78, 0x00, 0xC0, 0x00, 0x60, 0x10, 0x70, 0x90, 0x78, 0x00, // 0x84, 0x85
    0x30, 0x00, 0x60, 0x10, 0x70, 0x90, 0x78, 0x00, 0x00, 0x78, 0xC0, 0xC0, 0x78, 0x10, 0x30, 0x00, // 0x86, 0x87
    0xF8, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x78, 0x00, 0x88, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x78, 0x00, // 0x88, 0x89
    0xC0, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x78, 0x00, 0x28, 0x00, 0x30, 0x10, 0x10, 0x10, 0x38, 0x00, // 0x8A, 0x8B
    0x30, 0x48, 0x30, 0x10, 0x10, 0x10, 0x38, 0x00, 0x60, 0x00, 0x30, 0x10, 0x10, 0x10, 0x38, 0x00, // 0x8C, 0x8D
    0x50, 0x00, 0x20, 0x50, 0x88, 0xF8, 0x88, 0x88, 0x20, 0x00, 0x20, 0x50, 0x88, 0xF8, 0x88, 0x88, // 0x8E, 0x8F
    0x30, 0x00, 0xF0, 0x80, 0xE0, 0x80, 0xF0, 0x00, 0x00, 0x00, 0x78, 0x10, 0x78, 0x90, 0x78, 0x00, // 0x90, 0x91
    0x38, 0x50, 0x90, 0xF8, 0x90, 0x90, 0x98, 0x00, 0x70, 0x88, 0x00, 0x70, 0x88, 0x88, 0x70, 0x00, // 0x92, 0x93
    0x00, 0x88, 0x00, 0x70, 0x88, 0x88, 0x70, 0x00, 0x00, 0xC0, 0x00, 0x70, 0x88, 0x88, 0x70, 0x00, // 0x94, 0x95
    0x70, 0x88, 0x00, 0x88, 0x88, 0x98, 0x68, 0x00, 0x00, 0xC0, 0x00, 0x88, 0x88, 0x98, 0x68, 0x00, // 0x96, 0x97
    0x48, 0x00, 0x48, 0x48, 0x48, 0x38, 0x08, 0x70, 0x88, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, // 0x98, 0x99
    0x88, 0x00, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x20, 0x20, 0xF8, 0xA0, 0xA0, 0xF8, 0x20, 0x20, // 0x9A, 0x9B
    0x30, 0x58, 0x48, 0xE0, 0x40, 0x48, 0xF8, 0x00, 0xD8, 0xD8, 0x70, 0xF8, 0x20, 0xF8, 0x20, 0x20, // 0x9C, 0x9D
    0xE0, 0x90, 0x90, 0xE0, 0x90, 0xB8, 0x90, 0x90, 0x18, 0x28, 0x20, 0x70, 0x20, 0x20, 0xA0, 0xC0, // 0x9E, 0x9F
    0x18, 0x00, 0x60, 0x10, 0x70, 0x90, 0x78, 0x00, 0x18, 0x00, 0x30, 0x10, 0x10, 0x10, 0x38, 0x00, // 0xA0, 0xA1
    0x00, 0x18, 0x00, 0x70, 0x88, 0x88, 0x70, 0x00, 0x00, 0x18, 0x00, 0x88, 0x88, 0x98, 0x68, 0x00, // 0xA2, 0xA3
    0x00, 0x78, 0x00, 0x70, 0x48, 0x48, 0x48, 0x00, 0xF8, 0x00, 0xC8, 0xE8, 0xB8, 0x98, 0x88, 0x00, // 0xA4, 0xA5
    0x70, 0x90, 0x90, 0x78, 0x00, 0xF8, 0x00, 0x00, 0x70, 0x88, 0x88, 0x70, 0x00, 0xF8, 0x00, 0x00, // 0xA6, 0xA7
    0x20, 0x00, 0x20, 0x60, 0x80, 0x88, 0x70, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x80, 0x80, 0x00, 0x00, // 0xA8, 0xA9
    0x00, 0x00, 0x00, 0xF8, 0x08, 0x08, 0x00, 0x00, 0x80, 0x88, 0x90, 0xB8, 0x48, 0x98, 0x20, 0x38, // 0xAA, 0xAB
    0x80, 0x88, 0x90, 0xA8, 0x58, 0xB8, 0x08, 0x08, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x00, // 0xAC, 0xAD
    0x00, 0x28, 0x50, 0xA0, 0x50, 0x28, 0x00, 0x00, 0x00, 0xA0, 0x50, 0x28, 0x50, 0xA0, 0x00, 0x00, // 0xAE, 0xAF
    0x20, 0x88, 0x20, 0x88, 0x20, 0x88, 0x20, 0x88, 0x50, 0xA8, 0x50, 0xA8, 0x50, 0xA8, 0x50, 0xA8, // 0xB0, 0xB1
    0xA8, 0x50, 0xA8, 0x50, 0xA8, 0x50, 0xA8, 0x50, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // 0xB2, 0xB3
    0x10, 0x10, 0x10, 0x10, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x10, 0xF0, 0x10, 0x10, 0x10, // 0xB4, 0xB5
    0x28, 0x28, 0x28, 0x28, 0xE8, 0x28, 0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x28, 0x28, 0x28, // 0xB6, 0xB7
    0x00, 0x00, 0xF0, 0x10, 0xF0, 0x10, 0x10, 0x10, 0x28, 0x28, 0xE8, 0x08, 0xE8, 0x28, 0x28, 0x28, // 0xB8, 0xB9
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x00, 0x00, 0xF8, 0x08, 0xE8, 0x28, 0x28, 0x28, // 0xBA, 0xBB
    0x28, 0x28, 0xE8, 0x08, 0xF8, 0x00, 0x00, 0x00, 0x28, 0x28, 0x28, 0x28, 0xF8, 0x00, 0x00, 0x00, // 0xBC, 0xBD
    0x10, 0x10, 0xF0, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x10, 0x10, // 0xBE, 0xBF
    0x10, 0x10, 0x10, 0x10, 0x18, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0xF8, 0x00, 0x00, 0x00, // 0xC0, 0xC1
    0x00, 0x00, 0x00, 0x00, 0xF8, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18, 0x10, 0x10, 0x10, // 0xC2, 0xC3
    0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0xF8, 0x10, 0x10, 0x10, // 0xC4, 0xC5
    0x10, 0x10, 0x18, 0x10, 0x18, 0x10, 0x10, 0x10, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, // 0xC6, 0xC7
    0x28, 0x28, 0x28, 0x20, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x20, 0x28, 0x28, 0x28, 0x28, // 0xC8, 0xC9
    0x28, 0x28, 0xE8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xE8, 0x28, 0x28, 0x28, // 0xCA, 0xCB
    0x28, 0x28, 0x28, 0x20, 0x28, 0x28, 0x28, 0x28, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, // 0xCC, 0xCD
    0x28, 0x28, 0xE8, 0x00, 0xE8, 0x28, 0x28, 0x28, 0x10, 0x10, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, // 0xCE, 0xCF
    0x28, 0x28, 0x28, 0x28, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x10, 0x10, 0x10, // 0xD0, 0xD1
    0x00, 0x00, 0x00, 0x00, 0xF8, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x38, 0x00, 0x00, 0x00, // 0xD2, 0xD3
    0x10, 0x10, 0x18, 0x10, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x10, 0x18, 0x10, 0x10, 0x10, // 0xD4, 0xD5
    0x00, 0x00, 0x00, 0x00, 0x38, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0xF8, 0x28, 0x28, 0x28, // 0xD6, 0xD7
    0x10, 0x10, 0xF8, 0x10, 0xF8, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00, // 0xD8, 0xD9
    0x00, 0x00, 0x00, 0x00, 0x18, 0x10, 0x10, 0x10, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, // 0xDA, 0xDB
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xF8, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, // 0xDC, 0xDD
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, // 0xDE, 0xDF
    0x00, 0x00, 0x68, 0x90, 0x90, 0x90, 0x68, 0x00, 0x00, 0x70, 0x98, 0xF0, 0x98, 0xF0, 0x80, 0x00, // 0xE0, 0xE1
    0x00, 0xF8, 0x98, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0xF8, 0x50, 0x50, 0x50, 0x50, 0x50, 0x00, // 0xE2, 0xE3
    0xF8, 0x88, 0x40, 0x20, 0x40, 0x88, 0xF8, 0x00, 0x00, 0x00, 0x78, 0x90, 0x90, 0x90, 0x60, 0x00, // 0xE4, 0xE5
    0x00, 0x50, 0x50, 0x50, 0x50, 0x68, 0xC0, 0x00, 0x00, 0xF8, 0xA0, 0x20, 0x20, 0x20, 0x20, 0x00, // 0xE6, 0xE7
    0xF8, 0x20, 0x70, 0x88, 0x88, 0x70, 0x20, 0xF8, 0x20, 0x50, 0x88, 0xF8, 0x88, 0x50, 0x20, 0x00, // 0xE8, 0xE9
    0x20, 0x50, 0x88, 0x88, 0x50, 0x50, 0xD8, 0x00, 0x30, 0x40, 0x30, 0x70, 0x88, 0x88, 0x70, 0x00, // 0xEA, 0xEB
    0x00, 0x00, 0x00, 0x70, 0xA8, 0xA8, 0x70, 0x00, 0x08, 0x70, 0x98, 0xA8, 0xA8, 0xC8, 0x70, 0x80, // 0xEC, 0xED
    0x70, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x70, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x00, // 0xEE, 0xEF
    0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00, 0xF8, 0x00, // 0xF0, 0xF1
    0x40, 0x20, 0x10, 0x20, 0x40, 0x00, 0xF8, 0x00, 0x10, 0x20, 0x40, 0x20, 0x10, 0x00, 0xF8, 0x00, // 0xF2, 0xF3
    0x38, 0x28, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xA0, 0xA0, 0xE0, // 0xF4, 0xF5
    0x30, 0x30, 0x00, 0xF8, 0x00, 0x30, 0x30, 0x00, 0x00, 0xE8, 0xB8, 0x00, 0xE8, 0xB8, 0x00, 0x00, // 0xF6, 0xF7
    0x70, 0xD8, 0xD8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x50, 0x20, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, // 0xF8, 0xF9
    0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0xA0, 0xA0, 0x60, 0x20, // 0xFA, 0xFB
    0x70, 0x48, 0x48, 0x48, 0x48, 0x00, 0x00, 0x00, 0x70, 0x18, 0x30, 0x60, 0x78, 0x00, 0x00, 0x00, // 0xFC, 0xFD
    0x00, 0x00, 0x78, 0x78, 0x78, 0x78, 0x00, 0x00
};

// Use S/W SPI as it is slow anyway as it writes a nibble at a time, so the H/W SPI won't really be much faster
#define ST7920_CS()
#define ST7920_NCS()
#define ST7920_WRITE_BYTE(a)     {spi_write((uint8_t)((a)&0xf0));spi_write((uint8_t)((a)<<4));wait_us(10);}
#define ST7920_WRITE_BYTES(p,l)  {for(uint8_t j=0;j<l;j++){spi_write(*p&0xf0);spi_write(*p<<4);p++;} wait_us(10); }
#define ST7920_SET_CMD()         {spi_write(0xf8);wait_us(10);}
#define ST7920_SET_DAT()         {spi_write(0xfa);wait_us(10);}
#define PAGE_HEIGHT 32  //512 byte framebuffer
#define WIDTH 128
#define HEIGHT 64
#define FB_SIZE WIDTH*HEIGHT/8

#define enable_key "enable"
#define clk_pin_key "clk"
#define mosi_pin_key "mosi"
//#define miso_pin_key "miso"

static void wait_us(uint32_t us)
{
    if(us >= 10000) {
        safe_sleep(us / 1000);
    } else {
        uint32_t st = benchmark_timer_start();
        while(benchmark_timer_as_us(benchmark_timer_elapsed(st)) < us) ;
    }
}

static void wait_ns(uint32_t ns)
{
    uint32_t st = benchmark_timer_start();
    while(benchmark_timer_as_ns(benchmark_timer_elapsed(st)) < ns) ;
}

REGISTER_MODULE(ST7920, ST7920::create)

bool ST7920::create(ConfigReader& cr)
{
    printf("DEBUG: configure ST7920 display\n");
    ST7920 *st = new ST7920();
    if(!st->configure(cr)) {
        printf("INFO: ST7920 not enabled\n");
        delete st;
    }
    return true;
}

ST7920::ST7920() : Module("st7920")
{
}

ST7920::~ST7920()
{
    if(fb != nullptr) {
        free(fb);
    }
    if(clk != nullptr) {
        delete clk;
    }
    if(mosi != nullptr) {
        delete mosi;
    }
}

bool ST7920::configure(ConfigReader& cr)
{
    ConfigReader::section_map_t m;
    if(!cr.get_section("st7920", m)) return false;

    if(!cr.get_bool(m, enable_key, false)) {
        return false;
    }

    std::string clk_pin = cr.get_string(m, clk_pin_key, "nc");
    clk = new Pin(clk_pin.c_str(), Pin::AS_OUTPUT_OFF); // set low on creation
    if(!clk->connected()) {
        printf("ERROR:config_st7920: spi clk pin %s is invalid\n", clk_pin.c_str());
        return false;
    }
    printf("DEBUG:config_st7920: spi clk pin: %s\n", clk->to_string().c_str());

    std::string mosi_pin = cr.get_string(m, mosi_pin_key, "nc");
    mosi = new Pin(mosi_pin.c_str(), Pin::AS_OUTPUT_OFF); // set low on creation
    if(!mosi->connected()) {
        printf("ERROR:config_st7920: spi mosi pin %s is invalid\n", mosi_pin.c_str());
        return false;
    }
    printf("DEBUG:config_st7920: spi mosi pin: %s\n", mosi->to_string().c_str());


    fb = (uint8_t *)malloc(FB_SIZE); // grab some memory for the frame buffer
    if(fb == nullptr) {
        printf("ERROR:config_st7920: Not enough memory available for frame buffer");
        return false;
    }

    return true;
}

void ST7920::spi_write(uint8_t v)
{
    for (int b = 7; b >= 0; --b) {
        mosi->set(((v >> b) & 0x01) != 0);
        wait_ns(40);
        clk->set(true);
        wait_ns(300);
        clk->set(false);
        wait_ns(300);
    }
}

void ST7920::initDisplay()
{
    if(fb == NULL) return;

    ST7920_CS();
    clearScreen();  // clear framebuffer
    wait_us(90000);                 //initial delay for boot up
    ST7920_SET_CMD();
    ST7920_WRITE_BYTE(0x08);       //display off, cursor+blink off
    ST7920_WRITE_BYTE(0x01);       //clear CGRAM ram
    wait_us(10000);                 //delay for cgram clear
    ST7920_WRITE_BYTE(0x3E);       //extended mode + gdram active
    for(int y = 0; y < HEIGHT / 2; y++) { //clear GDRAM
        ST7920_WRITE_BYTE(0x80 | y); //set y
        ST7920_WRITE_BYTE(0x80);     //set x = 0
        ST7920_SET_DAT();
        for(int i = 0; i < 2 * WIDTH / 8; i++) { //2x width clears both segments
            ST7920_WRITE_BYTE(0);
        }
        ST7920_SET_CMD();
    }
    ST7920_WRITE_BYTE(0x0C); //display on, cursor+blink off
    ST7920_NCS();
    inited = true;
}

int16_t ST7920::width(void) const
{
    return WIDTH;
}

int16_t ST7920::height(void) const
{
    return HEIGHT;
}

void ST7920::clearScreen()
{
    if(fb == NULL) return;
    memset(fb, 0, FB_SIZE);
    dirty = true;
}

// render into local screenbuffer
void ST7920::displayString(int row, int col, const char *ptr, int length)
{
    for (int i = 0; i < length; ++i) {
        displayChar(row, col, ptr[i]);
        col += 1;
    }
    dirty = true;
}

void ST7920::renderChar(uint8_t *f, char c, int ox, int oy)
{
    if(f == NULL) return;
    // using the specific font data where x is in one byte and y is in consecutive bytes
    // the x bits are left aligned and right padded
    int i = c * 8; // character offset in font array
    int o = ox % 8; // where in f byte does it go
    int a = oy * 16 + ox / 8; // start address in frame buffer
    int mask = ~0xF8 >> o; // mask off top bits
    int mask2 = ~0xF8 << (8 - o); // mask off bottom bits
    for(int y = 0; y < 8; y++) {
        int b = font5x8[i + y]; // get font byte
        f[a] &= mask; // clear top bits for font
        f[a] |= (b >> o); // or in the fonts 1 bits
        if(o >= 4) { // it spans two f bytes
            f[a + 1] &= mask2; // clear bottom bits for font
            f[a + 1] |= (b << (8 - o)); // or in the fonts 1 bits
        }
        a += 16; // next line
    }
}

void ST7920::displayChar(int row, int col, char c)
{
    int x = col * 6;
    // if this wraps the line ignore it
    if(x + 6 > WIDTH) return;

    // convert row/column into y and x pixel positions based on font size
    renderChar(this->fb, c, x, row * 8);
}

void ST7920::renderGlyph(int xp, int yp, const uint8_t *g, int pixelWidth, int pixelHeight)
{
    if(fb == NULL) return;
    // NOTE the source is expected to be byte aligned and the exact number of pixels
    // TODO need to optimize by copying bytes instead of pixels...
    int xf = xp % 8;
    int rf = pixelWidth % 8;
    int a = yp * 16 + xp / 8; // start address in frame buffer
    const uint8_t *src = g;
    if(xf == 0) {
        // If xp is on a byte boundary simply memcpy each line from source to dest
        uint8_t *dest = &fb[a];
        int n = pixelWidth / 8; // bytes per line to copy
        if(rf != 0) n++; // if not a multiple of 8 pixels copy last byte as a byte
        if(n > 0) {
            for(int y = 0; y < pixelHeight; y++) {
                memcpy(dest, src, n);
                src += n;
                dest += 16; // next line
            }
        }

        // TODO now handle ragged end if we have one but as we always render left to right we probably don't need to
        // if(rf != 0) {

        // }
        dirty = true;
        return;
    }

    // if xp is not on a byte boundary we do the slow pixel by pixel copy
    int b = *g++;
    for(int y = 0; y < pixelHeight; y++) {
        int m = 0x80;
        for(int x = 0; x < pixelWidth; x++) {
            a = (y + yp) * 16 + (x + xp) / 8;
            int p = 1 << (7 - (x + xp) % 8);
            if((b & m) != 0) {
                fb[a] |= p;
            } else {
                fb[a] &= ~p;
            }
            m = m >> 1;
            if(m == 0) {
                m = 0x80;
                b = *g++;
            }
        }
    }
    dirty = true;
}

// displays a selectable rectangle from the glyph
void ST7920::bltGlyph(int x, int y, int w, int h, const uint8_t *glyph, int span, int x_offset, int y_offset)
{
    if(x_offset == 0 && y_offset == 0 && span == 0) {
        // blt the whole thing
        renderGlyph(x, y, glyph, w, h);

    } else {
        // copy portion of glyph into g where x_offset is left byte aligned
        // Note currently the x_offset must be byte aligned
        int n = w / 8; // bytes per line to copy
        if(w % 8 != 0) n++; // round up to next byte
        uint8_t g[n * h];
        uint8_t *dst = g;
        const uint8_t *src = &glyph[y_offset * span + x_offset / 8];
        for (int i = 0; i < h; ++i) {
            memcpy(dst, src, n);
            dst += n;
            src += span;
        }
        renderGlyph(x, y, g, w, h);
    }
}

// copy frame buffer to graphic buffer on display
void ST7920::fillGDRAM(const uint8_t *bitmap)
{
    unsigned char i, y;
    for ( i = 0 ; i < 2 ; i++ ) {
        ST7920_CS();
        for ( y = 0 ; y < PAGE_HEIGHT ; y++ ) {
            ST7920_SET_CMD();
            ST7920_WRITE_BYTE(0x80 | y);
            if ( i == 0 ) {
                ST7920_WRITE_BYTE(0x80);
            } else {
                ST7920_WRITE_BYTE(0x80 | 0x08);
            }
            ST7920_SET_DAT();
            ST7920_WRITE_BYTES(bitmap, WIDTH / 8); // bitmap gets incremented in this macro
        }
        ST7920_NCS();
    }
}

void ST7920::refresh()
{
    if(!inited || !dirty) return;
    fillGDRAM(this->fb);
    dirty = false;
}

void ST7920::drawByte(int index, uint8_t mask, int color)
{
    if (color == 1) {
        fb[index] |= mask;
    } else if (color == 0) {
        fb[index] &= ~mask;
    } else {
        fb[index] ^= mask;
    }
    dirty = true;
}

void ST7920::pixel(int x, int y, int color)
{
    if (y < HEIGHT && x < WIDTH) {
        unsigned char mask = 0x80 >> (x % 8);
        drawByte(y * (WIDTH / 8) + (x / 8), mask, color);
    }
}

void ST7920::drawHLine(int x, int y, int w, int color)
{
    for (int i = 0; i < w; i++) {
        pixel(x + i,  y,  color);
    }
}

void ST7920::drawVLine(int x, int y, int h, int color)
{
    for (int i = 0; i < h; i++) {
        pixel(x,  y + i,  color);
    }
}

void ST7920::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, int color)
{
    drawHLine(x, y, w, color);
    drawHLine(x, y + h - 1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x + w - 1, y, h, color);
}

void ST7920::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, int color)
{
    for (int16_t i = x; i < x + w; i++) {
        drawVLine(i, y, h, color);
    }
}

// Handle AdaFruit fonts

// modified code from Adafruit GFX library
int ST7920::drawAFChar(int x, int y, uint8_t c, int color)
{
    if(gfxFont == nullptr) return 0;

    c -= (uint8_t)gfxFont->first;
    GFXglyph *glyph = gfxFont->glyph + c;
    uint8_t *bitmap = gfxFont->bitmap;

    uint16_t bo = glyph->bitmapOffset;
    uint8_t w = glyph->width, h = glyph->height;
    int8_t xo = glyph->xOffset, yo = glyph->yOffset;
    uint8_t xx, yy, bits = 0, bit = 0;

    for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
            if (!(bit++ & 7)) {
                bits = bitmap[bo++];
            }
            if (bits & 0x80) {
                pixel(x + xo + xx, y + yo + yy, color);
            }
            bits <<= 1;
        }
    }
    return xo + w; // return width of character
}

void ST7920::displayAFString(int x, int y, int color, const char *ptr, int length)
{
    if(length == 0) length = strnlen(ptr, 64);
    for (int i = 0; i < length; ++i) {
        int w = drawAFChar(x, y, ptr[i], color);
        x += (w + 1);
    }
    dirty = true;
}

void ST7920::charBounds(unsigned char c, int16_t *x, int16_t *y,
                        int16_t *minx, int16_t *miny, int16_t *maxx,
                        int16_t *maxy)
{
    if (gfxFont == nullptr) return;
    int textsize_x = 1;
    int textsize_y = 1;
    int _width = WIDTH;
    bool wrap = true;

    uint8_t first = gfxFont->first,
            last = gfxFont->last;
    if ((c >= first) && (c <= last)) { // Char present in this font?
        GFXglyph *glyph = gfxFont->glyph + (c - first);
        uint8_t gw = glyph->width,
                gh = glyph->height,
                xa = glyph->xAdvance;
        int8_t xo = glyph->xOffset,
               yo = glyph->yOffset;
        if (wrap && ((*x + (((int16_t)xo + gw) * textsize_x)) > _width)) {
            *x = 0; // Reset x to zero, advance y by one line
            *y += textsize_y * (uint8_t)gfxFont->yAdvance;
        }
        int16_t tsx = (int16_t)textsize_x, tsy = (int16_t)textsize_y,
                x1 = *x + xo * tsx, y1 = *y + yo * tsy, x2 = x1 + gw * tsx - 1,
                y2 = y1 + gh * tsy - 1;
        if (x1 < *minx)
            *minx = x1;
        if (y1 < *miny)
            *miny = y1;
        if (x2 > *maxx)
            *maxx = x2;
        if (y2 > *maxy)
            *maxy = y2;
        *x += xa * tsx;
    }
}

void ST7920::getTextBounds(const char *str, int16_t x, int16_t y,
                           int16_t *x1, int16_t *y1, uint16_t *w,
                           uint16_t *h)
{

    uint8_t c; // Current character
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1; // Bound rect
    // Bound rect is intentionally initialized inverted, so 1st char sets it

    *x1 = x; // Initial position is value passed in
    *y1 = y;
    *w = *h = 0; // Initial size is zero

    while ((c = *str++)) {
        // charBounds() modifies x/y to advance for each character,
        // and min/max x/y are updated to incrementally build bounding rect.
        charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    }

    if (maxx >= minx) {     // If legit string bounds were found...
        *x1 = minx;           // Update x1 to least X coord,
        *w = maxx - minx + 1; // And w to bound rect width
    }
    if (maxy >= miny) { // Same for height
        *y1 = miny;
        *h = maxy - miny + 1;
    }
}
