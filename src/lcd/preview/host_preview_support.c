#include "host_preview.h"

#include <stdio.h>
#include <stdlib.h>

#include "LCD_1in3.h"

PAINT Paint;
LCD_1IN3_ATTRIBUTES LCD_1IN3 = {LCD_1IN3_WIDTH, LCD_1IN3_HEIGHT, HORIZONTAL};

static void paint_draw_pixel_block(UWORD xpoint, UWORD ypoint, UWORD color, UWORD size) {
    for (UWORD y = 0; y < size; ++y) {
        for (UWORD x = 0; x < size; ++x) {
            Paint_SetPixel(xpoint + x, ypoint + y, color);
        }
    }
}

void Paint_NewImage(UBYTE *image, UWORD width, UWORD height, UWORD rotate, UWORD color) {
    Paint.Image = image;
    Paint.WidthMemory = width;
    Paint.HeightMemory = height;
    Paint.Width = width;
    Paint.Height = height;
    Paint.Rotate = rotate;
    Paint.Color = color;
    Paint.Scale = 65;
    Paint.WidthByte = width * 2;
    Paint.HeightByte = height;
}

void Paint_SetScale(UBYTE scale) {
    Paint.Scale = scale;
    Paint.WidthByte = Paint.WidthMemory * 2;
}

void Paint_SetRotate(UWORD rotate) {
    Paint.Rotate = rotate;
}

void Paint_Clear(UWORD color) {
    for (UWORD y = 0; y < Paint.HeightMemory; ++y) {
        for (UWORD x = 0; x < Paint.WidthMemory; ++x) {
            Paint_SetPixel(x, y, color);
        }
    }
}

void Paint_SetPixel(UWORD xpoint, UWORD ypoint, UWORD color) {
    UDOUBLE address;

    if (Paint.Image == NULL) {
        return;
    }

    if (xpoint >= Paint.WidthMemory || ypoint >= Paint.HeightMemory) {
        return;
    }

    address = (UDOUBLE)(ypoint * Paint.WidthByte) + ((UDOUBLE)xpoint * 2u);
    Paint.Image[address] = (UBYTE)((color >> 8) & 0xFFu);
    Paint.Image[address + 1u] = (UBYTE)(color & 0xFFu);
}

void Paint_DrawPoint(UWORD xpoint, UWORD ypoint, UWORD color, DOT_PIXEL dot_pixel, DOT_STYLE dot_style) {
    UWORD size = (UWORD)dot_pixel;

    (void)dot_style;

    if (size == 0u) {
        size = 1u;
    }

    paint_draw_pixel_block(xpoint, ypoint, color, size);
}

void Paint_DrawCircle(UWORD x_center, UWORD y_center, UWORD radius, UWORD color, DOT_PIXEL line_width, DRAW_FILL draw_fill) {
    int x_current = 0;
    int y_current = (int)radius;
    int error = 3 - ((int)radius << 1);

    if (draw_fill == DRAW_FILL_FULL) {
        while (x_current <= y_current) {
            for (int span = x_current; span <= y_current; ++span) {
                Paint_DrawPoint((UWORD)(x_center + x_current), (UWORD)(y_center + span), color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
                Paint_DrawPoint((UWORD)(x_center - x_current), (UWORD)(y_center + span), color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
                Paint_DrawPoint((UWORD)(x_center - span), (UWORD)(y_center + x_current), color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
                Paint_DrawPoint((UWORD)(x_center - span), (UWORD)(y_center - x_current), color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
                Paint_DrawPoint((UWORD)(x_center - x_current), (UWORD)(y_center - span), color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
                Paint_DrawPoint((UWORD)(x_center + x_current), (UWORD)(y_center - span), color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
                Paint_DrawPoint((UWORD)(x_center + span), (UWORD)(y_center - x_current), color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
                Paint_DrawPoint((UWORD)(x_center + span), (UWORD)(y_center + x_current), color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
            }

            if (error < 0) {
                error += 4 * x_current + 6;
            } else {
                error += 10 + 4 * (x_current - y_current);
                --y_current;
            }
            ++x_current;
        }
        return;
    }

    while (x_current <= y_current) {
        Paint_DrawPoint((UWORD)(x_center + x_current), (UWORD)(y_center + y_current), color, line_width, DOT_FILL_AROUND);
        Paint_DrawPoint((UWORD)(x_center - x_current), (UWORD)(y_center + y_current), color, line_width, DOT_FILL_AROUND);
        Paint_DrawPoint((UWORD)(x_center - y_current), (UWORD)(y_center + x_current), color, line_width, DOT_FILL_AROUND);
        Paint_DrawPoint((UWORD)(x_center - y_current), (UWORD)(y_center - x_current), color, line_width, DOT_FILL_AROUND);
        Paint_DrawPoint((UWORD)(x_center - x_current), (UWORD)(y_center - y_current), color, line_width, DOT_FILL_AROUND);
        Paint_DrawPoint((UWORD)(x_center + x_current), (UWORD)(y_center - y_current), color, line_width, DOT_FILL_AROUND);
        Paint_DrawPoint((UWORD)(x_center + y_current), (UWORD)(y_center - x_current), color, line_width, DOT_FILL_AROUND);
        Paint_DrawPoint((UWORD)(x_center + y_current), (UWORD)(y_center + x_current), color, line_width, DOT_FILL_AROUND);

        if (error < 0) {
            error += 4 * x_current + 6;
        } else {
            error += 10 + 4 * (x_current - y_current);
            --y_current;
        }
        ++x_current;
    }
}

void Paint_DrawChar(UWORD xpoint, UWORD ypoint, char ascii_char, sFONT *font, UWORD color_foreground, UWORD color_background) {
    UWORD page;
    UWORD column;
    uint32_t bytes_per_row;
    uint32_t char_offset;
    const unsigned char *ptr;

    if (ascii_char < ' ' || font == NULL) {
        return;
    }

    bytes_per_row = (uint32_t)(font->Width / 8u) + ((font->Width % 8u) ? 1u : 0u);
    char_offset = (uint32_t)(ascii_char - ' ') * font->Height * bytes_per_row;
    ptr = &font->table[char_offset];

    for (page = 0; page < font->Height; ++page) {
        for (column = 0; column < font->Width; ++column) {
            if ((*ptr & (0x80u >> (column % 8u))) != 0u) {
                Paint_SetPixel(xpoint + column, ypoint + page, color_foreground);
            } else if (color_background != FONT_BACKGROUND) {
                Paint_SetPixel(xpoint + column, ypoint + page, color_background);
            }

            if ((column % 8u) == 7u) {
                ++ptr;
            }
        }

        if ((font->Width % 8u) != 0u) {
            ++ptr;
        }
    }
}

void Paint_DrawString_EN(UWORD xstart, UWORD ystart, const char *string, sFONT *font, UWORD color_foreground, UWORD color_background) {
    UWORD xpoint = xstart;
    UWORD ypoint = ystart;

    if (string == NULL || font == NULL) {
        return;
    }

    while (*string != '\0') {
        if ((UWORD)(xpoint + font->Width) > Paint.Width) {
            xpoint = xstart;
            ypoint = (UWORD)(ypoint + font->Height);
        }

        if ((UWORD)(ypoint + font->Height) > Paint.Height) {
            xpoint = xstart;
            ypoint = ystart;
        }

        Paint_DrawChar(xpoint, ypoint, *string, font, color_background, color_foreground);
        ++string;
        xpoint = (UWORD)(xpoint + font->Width);
    }
}

void Paint_DrawImage(const unsigned char *image, UWORD x_start, UWORD y_start, UWORD width, UWORD height) {
    for (UWORD y = 0; y < height; ++y) {
        for (UWORD x = 0; x < width; ++x) {
            size_t offset = ((size_t)y * (size_t)width * 2u) + ((size_t)x * 2u);
            UWORD color = (UWORD)(((UWORD)image[offset + 1u] << 8) | (UWORD)image[offset]);
            Paint_SetPixel(x_start + x, y_start + y, color);
        }
    }
}

void LCD_1IN3_Display(UWORD *image) {
    (void)image;
}

void LCD_1IN3_DisplayWindows(UWORD xstart, UWORD ystart, UWORD xend, UWORD yend, UWORD *image) {
    (void)xstart;
    (void)ystart;
    (void)xend;
    (void)yend;
    (void)image;
}

static void rgb565_to_rgb888(UWORD color, unsigned char *rgb) {
    unsigned int red = (unsigned int)((color >> 11) & 0x1Fu);
    unsigned int green = (unsigned int)((color >> 5) & 0x3Fu);
    unsigned int blue = (unsigned int)(color & 0x1Fu);

    rgb[0] = (unsigned char)((red * 255u) / 31u);
    rgb[1] = (unsigned char)((green * 255u) / 63u);
    rgb[2] = (unsigned char)((blue * 255u) / 31u);
}

int host_preview_write_ppm(const char *path, const UBYTE *image, UWORD width, UWORD height) {
    FILE *file = fopen(path, "wb");

    if (file == NULL) {
        perror(path);
        return -1;
    }

    if (fprintf(file, "P6\n%u %u\n255\n", width, height) < 0) {
        fclose(file);
        return -1;
    }

    for (UWORD y = 0; y < height; ++y) {
        for (UWORD x = 0; x < width; ++x) {
            size_t offset = ((size_t)y * (size_t)width * 2u) + ((size_t)x * 2u);
            UWORD color = (UWORD)(((UWORD)image[offset] << 8) | (UWORD)image[offset + 1u]);
            unsigned char rgb[3];

            rgb565_to_rgb888(color, rgb);
            if (fwrite(rgb, sizeof(rgb), 1u, file) != 1u) {
                fclose(file);
                return -1;
            }
        }
    }

    fclose(file);
    return 0;
}
