#include <stdint.h>
#include "graphics.h"

#define FB_WIDTH 320U
#define FB_HEIGHT 200U

static uint32_t fb_backing[FB_WIDTH * FB_HEIGHT];

struct framebuffer fb;

void graphics_init(void) {
    fb.addr = fb_backing;
    fb.width = FB_WIDTH;
    fb.height = FB_HEIGHT;
    fb.pitch = FB_WIDTH * sizeof(uint32_t);
}

void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x < fb.width && y < fb.height) {
        fb.addr[(y * (fb.pitch / 4U)) + x] = color;
    }
}
