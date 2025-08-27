#include "render.h"
#include "window.h"
#include <stddef.h>  // For NULL
#include <stdint.h>  // For uint32_t etc.
#include "time.h"
#include <stdbool.h>
#include "keyboard.h"
#include "shell.h"
#include "string.h"

#define MAX_LINES 22
#define MAX_LINE_LENGTH 60

// Multiboot2 tag types
#define MULTIBOOT2_TAG_FRAMEBUFFER 8

// Framebuffer type
#define MULTIBOOT2_FRAMEBUFFER_TYPE_RGB 1

// Multiboot2 structures
struct multiboot2_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot2_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t reserved;
};

// Function to find a multiboot2 tag
void* find_multiboot2_tag(void* mb_info, uint32_t type) {
    if (!mb_info) return NULL;
    
    struct multiboot2_tag* tag = (struct multiboot2_tag*)((uint8_t*)mb_info + 8);
    
    while (tag->type != 0) { // 0 is the end tag
        if (tag->type == type) {
            return tag;
        }
        
        // Move to the next tag (align to 8 bytes)
        tag = (struct multiboot2_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }
    
    return NULL;
}

void Start(){
}

void Update(){
}

void terminal_main(Window* window) {
    static char input_buffer[MAX_LINE_LENGTH] = {0};
    static int input_len = 0;
    static char output_lines[MAX_LINES][MAX_LINE_LENGTH] = {{0}};
    static int output_count = 0;
    static int scroll_offset = 0;

    // Get keyboard input
    char c = get_key();
    if (c != '\0') {
        if (c == '\b' && input_len > 0) {
            input_len--;
            input_buffer[input_len] = '\0';
        } else if (c == '\n') {
            input_buffer[input_len] = '\0';
            
            // Add input line to output
            if (output_count < MAX_LINES) {
                strncpy(output_lines[output_count], "> ", 2);
                strncat(output_lines[output_count], input_buffer, MAX_LINE_LENGTH - 3);
                output_count++;
            } else {
                // Scroll up
                for (int i = 0; i < MAX_LINES - 1; i++) {
                    strncpy(output_lines[i], output_lines[i+1], MAX_LINE_LENGTH);
                }
                strncpy(output_lines[MAX_LINES-1], "> ", 2);
                strncat(output_lines[MAX_LINES-1], input_buffer, MAX_LINE_LENGTH - 3);
            }
            
            // Process command and add output
            const char* result = shell(input_buffer);
            if (result) {
                // Simple line splitting without strtok
                const char* start = result;
                const char* end = result;
                while (*end) {
                    if (*end == '\n') {
                        // Found a newline, add the line
                        int len = end - start;
                        if (len > 0 && len < MAX_LINE_LENGTH) {
                            if (output_count < MAX_LINES) {
                                strncpy(output_lines[output_count], start, len);
                                output_lines[output_count][len] = '\0';
                                output_count++;
                            } else {
                                // Scroll up
                                for (int i = 0; i < MAX_LINES - 1; i++) {
                                    strncpy(output_lines[i], output_lines[i+1], MAX_LINE_LENGTH);
                                }
                                strncpy(output_lines[MAX_LINES-1], start, len);
                                output_lines[MAX_LINES-1][len] = '\0';
                            }
                        }
                        start = end + 1;
                    }
                    end++;
                }
                
                // Add the last line if there's any remaining
                if (start < end) {
                    int len = end - start;
                    if (len > 0 && len < MAX_LINE_LENGTH) {
                        if (output_count < MAX_LINES) {
                            strncpy(output_lines[output_count], start, len);
                            output_lines[output_count][len] = '\0';
                            output_count++;
                        } else {
                            // Scroll up
                            for (int i = 0; i < MAX_LINES - 1; i++) {
                                strncpy(output_lines[i], output_lines[i+1], MAX_LINE_LENGTH);
                            }
                            strncpy(output_lines[MAX_LINES-1], start, len);
                            output_lines[MAX_LINES-1][len] = '\0';
                        }
                    }
                }
            }
            
            input_len = 0;
            input_buffer[0] = '\0';
            
            // Auto-scroll to bottom
            scroll_offset = 0;
        } else if (input_len < MAX_LINE_LENGTH - 1 && c >= 32 && c <= 126) {
            input_buffer[input_len++] = c;
            input_buffer[input_len] = '\0';
        }
    }

    // Calculate available space
    int content_width = window->size_x - 2;
    int content_height = window->size_y - 45;
    int line_height = 14;
    int max_visible_lines = content_height / line_height;

    // Handle scrolling
    int start_line = 0;
    if (output_count > max_visible_lines) {
        start_line = output_count - max_visible_lines - scroll_offset;
        if (start_line < 0) {
            scroll_offset += start_line;
            start_line = 0;
        }
    }

    // Clear content area
    window_draw_rect(window, 0, 0, content_width, content_height, 0x000000, true);

    // Draw output lines
    for (int i = 0; i < max_visible_lines && (start_line + i) < output_count; i++) {
        window_draw_text(window, output_lines[start_line + i], 
                        5, 5 + i * line_height, 0xFFFFFF, 1);
    }

    // Draw input area
    window_draw_rect(window, 0, window->size_y - 45, content_width, 25, 0x333333, true);
    window_draw_text(window, "> ", 0, window->size_y - 35, 0xFFFFFF, 1);
    window_draw_text(window, input_buffer, 15, window->size_y - 35, 0xFFFFFF, 1);

    // Draw cursor
    static int blink = 0;
    blink = (blink + 1) % 60;
    if (blink < 30) {
        int cursor_x = 15 + input_len * 8;
        if (cursor_x < content_width) {
            window_draw_rect(window, cursor_x, window->size_y - 38, 6, 12, 0xFFFFFF, true);
        }
    }
}


void kernel_main(uint32_t magic, void* mb_info) {
    // Check if we were loaded by a Multiboot2-compliant bootloader
    if (magic != 0x36d76289) {
        // Not loaded by Multiboot2, can't continue
        return;
    }

    // Find framebuffer tag
    struct multiboot2_tag_framebuffer* fb_tag = find_multiboot2_tag(mb_info, MULTIBOOT2_TAG_FRAMEBUFFER);
    if (!fb_tag || fb_tag->framebuffer_type != MULTIBOOT2_FRAMEBUFFER_TYPE_RGB) {
        return;
    }

    // Initialize renderer with framebuffer info
    // Cast the 64-bit address to 32-bit pointer (we're in 32-bit mode)
    render_init((uint32_t*)(uintptr_t)fb_tag->framebuffer_addr,
               fb_tag->framebuffer_width,
               fb_tag->framebuffer_height,
               fb_tag->framebuffer_pitch);


    Window* myWin = window_create(350, 250, 50, 50);
    myWin->title = "Terminal";
    myWin->color = 0x446688;
    myWin->draw = terminal_main;

    time_init(1000000);  // 1MHz
    
    Start();

    while (1) {

        clear_screen(0xF000);
        
        // Draw all windows
        window_draw_all();

        render_present();

        Update();

    }
}