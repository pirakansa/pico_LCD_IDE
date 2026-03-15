#ifndef HOST_PREVIEW_GUI_PAINT_H
#define HOST_PREVIEW_GUI_PAINT_H

#include <stdint.h>

#include "fonts.h"

typedef uint8_t UBYTE;
typedef uint16_t UWORD;
typedef uint32_t UDOUBLE;

typedef struct {
    UBYTE *Image;
    UWORD Width;
    UWORD Height;
    UWORD WidthMemory;
    UWORD HeightMemory;
    UWORD Color;
    UWORD Rotate;
    UWORD Mirror;
    UWORD WidthByte;
    UWORD HeightByte;
    UWORD Scale;
} PAINT;

extern PAINT Paint;

#define ROTATE_0 0

#define WHITE 0xFFFF
#define BLACK 0x0000
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define BROWN 0xBC40

#define FONT_BACKGROUND WHITE

typedef enum {
    DOT_PIXEL_1X1 = 1,
    DOT_PIXEL_2X2,
    DOT_PIXEL_3X3,
    DOT_PIXEL_4X4,
    DOT_PIXEL_5X5,
    DOT_PIXEL_6X6,
    DOT_PIXEL_7X7,
    DOT_PIXEL_8X8,
} DOT_PIXEL;

typedef enum {
    DOT_FILL_AROUND = 1,
    DOT_FILL_RIGHTUP,
} DOT_STYLE;

typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
} DRAW_FILL;

void Paint_NewImage(UBYTE *image, UWORD width, UWORD height, UWORD rotate, UWORD color);
void Paint_SetScale(UBYTE scale);
void Paint_SetRotate(UWORD rotate);
void Paint_Clear(UWORD color);
void Paint_SetPixel(UWORD xpoint, UWORD ypoint, UWORD color);
void Paint_DrawPoint(UWORD xpoint, UWORD ypoint, UWORD color, DOT_PIXEL dot_pixel, DOT_STYLE dot_style);
void Paint_DrawCircle(UWORD x_center, UWORD y_center, UWORD radius, UWORD color, DOT_PIXEL line_width, DRAW_FILL draw_fill);
void Paint_DrawChar(UWORD xpoint, UWORD ypoint, char ascii_char, sFONT *font, UWORD color_foreground, UWORD color_background);
void Paint_DrawString_EN(UWORD xstart, UWORD ystart, const char *string, sFONT *font, UWORD color_foreground, UWORD color_background);
void Paint_DrawImage(const unsigned char *image, UWORD x_start, UWORD y_start, UWORD width, UWORD height);

#endif
