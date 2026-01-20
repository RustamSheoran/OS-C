#include "graphics.h"

struct framebuffer fb;

void graphics_init() {

    // Stub: assume framebuffer at 0xE0000000

    fb.addr = (uint32_t *)0xE0000000;

    fb.width = 1024;

    fb.height = 768;

    fb.pitch = 1024 * 4;

}

void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {

    if (x < fb.width && y < fb.height) {

        fb.addr[y * (fb.pitch / 4) + x] = color;

    }

}