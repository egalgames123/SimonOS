// window.h
#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Window Window;

// Window callback functions
typedef void (*WindowDrawFunc)(Window* window);
typedef void (*WindowLogicFunc)(Window* window);

struct Window {
    uint32_t pos_x, pos_y;
    uint32_t size_x, size_y;
    uint32_t color;
    const char* title;
    WindowDrawFunc draw;
    bool visible;
    void* user_data; // For custom window data
};

// Window creation/destruction
Window* window_create(uint32_t size_x, uint32_t size_y, uint32_t pos_x, uint32_t pos_y);
void window_destroy(Window* window);

// Window management
void window_draw_all();
void window_update_all();

// Helper functions
void window_draw_text(Window* window, const char* text, uint32_t x, uint32_t y, uint32_t color, uint32_t scale);
void window_draw_rect(Window* window, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color, bool filled);

#endif // WINDOW_H