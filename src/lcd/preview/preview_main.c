#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_preview.h"
#include "../DrawData.h"

static int render_preview(const char *output_dir, const char *name, int selected) {
    char path[512];
    UBYTE *image = (UBYTE *)malloc((size_t)LCD_1IN3_WIDTH * (size_t)LCD_1IN3_HEIGHT * 2u);

    if (image == NULL) {
        fprintf(stderr, "failed to allocate preview buffer\n");
        return -1;
    }

    Paint_NewImage(image, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);

    if (strcmp(name, "splash") == 0) {
        draw_splash_screen((UWORD *)image);
    } else {
        draw_menu_screen((UWORD *)image);
        draw_radio_menu_screen((UWORD *)image, selected);
    }

    if (snprintf(path, sizeof(path), "%s/%s.ppm", output_dir, name) >= (int)sizeof(path)) {
        fprintf(stderr, "preview path is too long\n");
        free(image);
        return -1;
    }

    if (host_preview_write_ppm(path, image, LCD_1IN3.WIDTH, LCD_1IN3.HEIGHT) != 0) {
        free(image);
        return -1;
    }

    printf("%s\n", path);
    free(image);
    return 0;
}

int main(int argc, char **argv) {
    const char *output_dir = "build/ui-preview";

    if (argc > 1) {
        output_dir = argv[1];
    }

    if (render_preview(output_dir, "splash", -1) != 0) {
        return 1;
    }

    for (int selected = 0; selected < 4; ++selected) {
        char name[32];

        if (snprintf(name, sizeof(name), "menu_selected_%d", selected) >= (int)sizeof(name)) {
            fprintf(stderr, "preview name is too long\n");
            return 1;
        }

        if (render_preview(output_dir, name, selected) != 0) {
            return 1;
        }
    }

    return 0;
}
