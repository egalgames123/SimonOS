#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include <stdbool.h>

// Initialize renderer with framebuffer info
void render_init(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch);

// Basic drawing functions
void draw_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color, bool filled);
void draw_circle(uint32_t x, uint32_t y, uint32_t r, uint32_t color, bool filled);
void draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

// Text rendering
void draw_char(char c, uint32_t x, uint32_t y, uint32_t color, uint32_t bg_color, uint32_t scale);
void draw_text(const char* str, uint32_t x, uint32_t y, uint32_t color, uint32_t bg_color, uint32_t scale);

// Screen operations
void clear_screen(uint32_t color);
void render_present(); // Useful if you implement double buffering later

#endif // RENDER_H