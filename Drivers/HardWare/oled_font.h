#ifndef __OLED_FONT_H_
#define __OLED_FONT_H_

#include "stdint.h"

typedef struct
{
    uint8_t Index[3];
    uint8_t Data[32];
} OLED_Character_t;

extern const uint8_t oled_font_6x8[][6];
extern const uint8_t oled_font_7x12[][14];
extern const uint8_t oled_font_8x16[][16];

extern const OLED_Character_t oled_Cfont_12x12[];
extern const OLED_Character_t oled_Cfont_16x16[];

#endif
