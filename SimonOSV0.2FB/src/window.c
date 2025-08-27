#include "window.h"
#include "render.h"
#include <stddef.h>
#include <string.h>

#define MAX_WINDOWS 16
static Window windows[MAX_WINDOWS];
static uint32_t window_count = 0;

Window* window_create(uint32_t size_x, uint32_t size_y, uint32_t pos_x, uint32_t pos_y) {
    if (window_count >= MAX_WINDOWS) return NULL;

    Window* win = &windows[window_count++];
    win->size_x = size_x;
    win->size_y = size_y;
    win->pos_x = pos_x;
    win->pos_y = pos_y;
    win->color = 0x333333; // Default color
    win->title = "Window";
    win->draw = NULL;
    win->visible = true;
    win->user_data = NULL;

    return win;
}

void window_destroy(Window* window) {
    if (!window) return;
    window->visible = false;
    // Ressourcenfreigabe ggf.
}

void window_clear(Window* window, uint32_t color) {
    if (!window || !window->visible) return;

    // Clear Fensterinhalt (ohne Titelleiste)
    draw_rect(window->pos_x + 1, window->pos_y + 21,
              window->size_x - 2, window->size_y - 22,
              color, true);
}

void window_draw_all() {
    for (uint32_t i = 0; i < window_count; i++) {
        Window* win = &windows[i];
        if (!win->visible) continue;

        // Fensterrahmen
        draw_rect(win->pos_x, win->pos_y, win->size_x, win->size_y, win->color, false);

        // Titelleiste (20px hoch)
        draw_rect(win->pos_x + 1, win->pos_y + 1, win->size_x - 2, 20, win->color, true);

        // Titel (immer Skalierung 1, da es klein bleiben soll)
        draw_text(win->title, win->pos_x + 5, win->pos_y + 5, 0xFFFFFF, 0xFFFFFFFF, 1);

        // Skalierung berechnen für Fensterinhalt (text-skalierung)

        // Grundwerte (Font 8x14 px)
        const int base_char_width = 8;
        const int base_char_height = 14;

        // Fensterhöhe ohne Titelleiste
        int content_height = (int)win->size_y - 20;
        int content_width = (int)win->size_x;

        // Max Anzahl Textzeilen im Fenster (ganzzahlig)
        int max_lines = content_height / base_char_height;

        // Max Zeichen pro Zeile (ganzzahlig)
        int max_chars = content_width / base_char_width;

        // Skalierung horizontal & vertikal berechnen
        float scale_x = (float)content_width / (max_chars * base_char_width);
        float scale_y = (float)content_height / (max_lines * base_char_height);

        // Take the smaller scale (keine Dehnung)
        float scale = scale_x < scale_y ? scale_x : scale_y;

        // Minimum Skalierung 1 (keine Verkleinerung unter 1)
        if (scale < 1.0f) scale = 1.0f;

        // Jetzt das Fenster zeichnen, indem die Fenster-spezifische draw Funktion mit scale aufgerufen wird
        if (win->draw) {
            // Speichere scale im user_data falls nötig
            win->user_data = (void*)(uintptr_t)(int)(scale * 1000); // Skala * 1000 als int speichern zB

            // Die Draw-Funktion erwartet, dass sie scale irgendwie nutzt (du kannst das anpassen)
            win->draw(win);
        }
    }
}

void window_draw_text(Window* window, const char* text, uint32_t x, uint32_t y, uint32_t color, uint32_t scale) {
    if (!window || !window->visible || !text) return;
    
    // Calculate absolute positions
    uint32_t abs_x = window->pos_x + 1 + x;
    uint32_t abs_y = window->pos_y + y + 20;  // +20 for title bar
    
    // Calculate maximum allowed position
    uint32_t max_x = window->pos_x + window->size_x;
    uint32_t max_y = window->pos_y + window->size_y;
    
    // Only draw if within window bounds
    if (abs_x < max_x && abs_y < max_y) {
        // Clip text width to not exceed window width
        uint32_t max_width = max_x - abs_x;
        if (max_width > 0) {
            draw_text(text, abs_x, abs_y, color, 0xFFFFFFFF, scale);
        }
    }
}

void window_draw_rect(Window* window, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color, bool filled) {
    if (!window || !window->visible) return;

    draw_rect(window->pos_x + 1 + x, window->pos_y + y + 20, w, h, color, filled);
}
