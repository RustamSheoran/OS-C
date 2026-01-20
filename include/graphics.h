#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

struct framebuffer {

    uint32_t *addr;

    uint32_t width;

    uint32_t height;

    uint32_t pitch;

};

extern struct framebuffer fb;

void graphics_init();

void draw_pixel(uint32_t x, uint32_t y, uint32_t color);

#endif