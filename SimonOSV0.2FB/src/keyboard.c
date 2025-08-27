#include "keyboard.h"
#include <stdint.h>

#define PS2_DATA 0x60
#define PS2_STATUS 0x64

// Modifier key flags
#define MOD_SHIFT     0x01
#define MOD_CTRL      0x02
#define MOD_ALT       0x04
#define MOD_CAPS      0x08
#define MOD_NUM       0x10
#define MOD_SCROLL    0x20

static uint8_t modifiers = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Regular scancode map (unshifted)
static const char scancode_map[] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',  0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0,
    // F1-F12 can be added later
};

// Shifted scancode map
static const char shifted_scancode_map[] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~',  0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ', 0,
};

char get_key() {
    // Only read if data is available
    if ((inb(PS2_STATUS) & 1) == 0) return '\0';

    uint8_t scancode = inb(PS2_DATA);

    // Handle key releases (ignore for now except modifier keys)
    if (scancode & 0x80) {
        uint8_t released_key = scancode & 0x7F;
        
        // Check for modifier key releases
        switch(released_key) {
            case 0x2A: // Left Shift
            case 0x36: // Right Shift
                modifiers &= ~MOD_SHIFT;
                break;
            case 0x1D: // Ctrl
                modifiers &= ~MOD_CTRL;
                break;
            case 0x38: // Alt
                modifiers &= ~MOD_ALT;
                break;
        }
        return '\0';
    }

    // Handle modifier key presses
    switch(scancode) {
        case 0x2A: // Left Shift
        case 0x36: // Right Shift
            modifiers |= MOD_SHIFT;
            return '\0';
        case 0x1D: // Ctrl
            modifiers |= MOD_CTRL;
            return '\0';
        case 0x38: // Alt
            modifiers |= MOD_ALT;
            return '\0';
        case 0x3A: // Caps Lock
            modifiers ^= MOD_CAPS; // Toggle caps lock
            return '\0';
    }

    // Check if we should use shifted map
    int shifted = (modifiers & MOD_SHIFT) ^ (modifiers & MOD_CAPS);
    
    if (scancode < sizeof(scancode_map)) {
        if (shifted && scancode < sizeof(shifted_scancode_map)) {
            return shifted_scancode_map[scancode];
        } else {
            return scancode_map[scancode];
        }
    }

    return '\0';
}