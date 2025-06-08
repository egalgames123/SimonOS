#include <stdint.h>

__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    0x1BADB002, 0x0, -(0x1BADB002)
};

volatile char* const VGA = (volatile char*)0xB8000;
int cursor = 0;

void update_cursor() {
    unsigned short pos = cursor / 2;

    __asm__ volatile ("outb %b0, %1" : : "a"(14), "Nd"(0x3D4));
    __asm__ volatile ("outb %b0, %1" : : "a"(pos >> 8), "Nd"(0x3D5));

    __asm__ volatile ("outb %b0, %1" : : "a"(15), "Nd"(0x3D4));
    __asm__ volatile ("outb %b0, %1" : : "a"(pos & 0xFF), "Nd"(0x3D5));
}

int strncmp(const char* a, const char* b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return (unsigned char)a[i] - (unsigned char)b[i];
        if (a[i] == '\0') return 0;
    }
    return 0;
}



void print(const char* s) {
    while (*s) {
        if (*s == '\n') {
            cursor += 160 - (cursor % 160);
        } else {
            VGA[cursor++] = *s;
            VGA[cursor++] = 0x07;
        }
        update_cursor();
        s++;
    }
}

void print_char(char c) {
    if (c == '\n') {
        cursor += 160 - (cursor % 160);
    } else {
        VGA[cursor++] = c;
        VGA[cursor++] = 0x07;
    }
    update_cursor();
}

void move_cursor_back() {
    if (cursor >= 2) {
        cursor -= 2;
        VGA[cursor] = ' ';
        VGA[cursor + 1] = 0x07;
    }
    update_cursor();
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline int kbd_data_available() {
    unsigned char status;
    __asm__ volatile ("inb %1, %0" : "=a"(status) : "Nd"(0x64));
    return status & 1;
}

unsigned char read_scancode() {
    while (!kbd_data_available()) {}
    return inb(0x60);
}

char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ',
};

void read_line(char* buffer, int max_len) {
    int i = 0;
    while (1) {
        unsigned char sc = read_scancode();

        if (sc & 0x80) continue;  // Ignore key releases

        char c = scancode_to_ascii[sc];

        if (c == '\n' || c == '\r') {
            print("\n");
            buffer[i] = 0;
            return;
        } 
        else if (c == '\b') {
            if (i > 0) {
                i--;
                move_cursor_back();
            }
        } 
        else if (c && i < max_len - 1) {
            buffer[i++] = c;
            print_char(c);
        }
    }

}

void clear_screen() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        VGA[i] = ' ';       // space character
        VGA[i + 1] = 0x07;  // attribute: light gray on black
    }
    cursor = 0;
    update_cursor();
}


void process_command(const char* input) {
    if (input[0] == 0) return;  // ignore empty

    // Simple strcmp implementation (no stdlib)
    int strcmp(const char* a, const char* b) {
        while (*a && *a == *b) { a++; b++; }
        return *(unsigned char*)a - *(unsigned char*)b;
    }

    if (strcmp(input, "help") == 0) {
        print("Commands:\n");
        print("help - show this message\n");
        print("echo - repeat input\n");
        print("clear - clear screen (not implemented)\n");
    }
    else if (strncmp(input, "echo ", 5) == 0) {
        print(input + 5);
        print("\n");
    }
    else if (strcmp(input, "clear") == 0) {
        clear_screen();
    }
    else {
        print("Unknown command: ");
        print(input);
        print("\n");
    }
}




void kernel_main() {
    print("SimonOS Shell v0.1\n");

    char input[128];

    while (1) {
    print("SimonOS > ");
    read_line(input, sizeof(input));

    process_command(input);
}

}
