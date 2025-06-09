#include <stdint.h>
#include "fs.h"

__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    0x1BADB002, 0x0, -(0x1BADB002)
};

volatile char* const VGA = (volatile char*)0xB8000;
int cursor = 0;

// Keyboard state tracking
int shift_pressed = 0;
int caps_lock = 0;

void update_cursor() {
    unsigned short pos = cursor / 2;
    __asm__ volatile ("outb %b0, %1" : : "a"(14), "Nd"(0x3D4));
    __asm__ volatile ("outb %b0, %1" : : "a"(pos >> 8), "Nd"(0x3D5));
    __asm__ volatile ("outb %b0, %1" : : "a"(15), "Nd"(0x3D4));
    __asm__ volatile ("outb %b0, %1" : : "a"(pos & 0xFF), "Nd"(0x3D5));
}

void scroll_if_needed() {
    if (cursor >= 80 * 25 * 2) {
        for (int row = 1; row < 25; row++) {
            for (int col = 0; col < 80 * 2; col++) {
                VGA[(row - 1) * 160 + col] = VGA[row * 160 + col];
            }
        }
        for (int col = 0; col < 80 * 2; col++) {
            VGA[24 * 160 + col] = (col % 2 == 0) ? ' ' : 0x07;
        }
        cursor -= 160;
    }
    update_cursor();
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
        scroll_if_needed();
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
    scroll_if_needed();
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
    'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ', 0
};

char scancode_to_shift_ascii[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','\"','~', 0,'|',
    'Z','X','C','V','B','N','M','<','>','?', 0,'*', 0,' ', 0
};

void update_keyboard_state(unsigned char scancode) {
    if ((scancode & 0x7F) == 0x2A || (scancode & 0x7F) == 0x36) {
        shift_pressed = !(scancode & 0x80);
    }
    else if (scancode == 0x3A) {
        caps_lock = !caps_lock;
    }
}

char get_ascii_char(unsigned char scancode) {
    if (scancode & 0x80) return 0;

    if (scancode < sizeof(scancode_to_ascii)) {
        char c = scancode_to_ascii[scancode];
        if (shift_pressed) return scancode_to_shift_ascii[scancode];
        if (caps_lock && c >= 'a' && c <= 'z') return scancode_to_shift_ascii[scancode];
        return c;
    }
    return 0;
}

void read_line(char* buffer, int max_len) {
    int i = 0;
    while (1) {
        unsigned char sc = read_scancode();
        update_keyboard_state(sc);
        char c = get_ascii_char(sc);

        if (c == '\n' || c == '\r') {
            print("\n");
            buffer[i] = 0;
            return;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                move_cursor_back();
            }
        } else if (c && i < max_len - 1) {
            buffer[i++] = c;
            print_char(c);
        }
    }
}

void clear_screen() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        VGA[i] = ' ';
        VGA[i + 1] = 0x07;
    }
    cursor = 0;
    update_cursor();
}

void process_command(const char* input) {
    if (input[0] == 0) return;

    int strcmp(const char* a, const char* b) {
        while (*a && *a == *b) { a++; b++; }
        return *(unsigned char*)a - *(unsigned char*)b;
    }

    if (strcmp(input, "help") == 0) {
        print("Commands:\n");
        print("help - show this message\n");
        print("echo <text> - repeat input\n");
        print("echo <text> > <file> - write to file\n");
        print("touch <file> - create file\n");
        print("rm <file> - delete file\n");
        print("clear - clear screen\n");
        print("ls - list files\n");
        print("cat <file> - show file content\n");
        print("version - shows SimonOS version\n");
    }
    else if (strncmp(input, "echo ", 5) == 0) {
        const char* arrow = " > ";
        char* split = 0;
        for (int i = 5; input[i]; i++) {
            if (input[i] == '>' && input[i + 1] == ' ') {
                split = (char*)(input + i);
                break;
            }
        }
        if (split) {
            *split = 0;
            const char* text = input + 5;
            const char* filename = split + 2;
            fs_create(filename);
            if (fs_write(filename, text)) {
                print("Written to ");
                print(filename);
                print("\n");
            } else {
                print("File not found.\n");
            }
        } else {
            print(input + 5);
            print("\n");
        }
    }
    else if (strcmp(input, "clear") == 0) {
        clear_screen();
    }
    else if (strcmp(input, "ls") == 0) {
        fs_list();
    }
    else if (strncmp(input, "cat ", 4) == 0) {
        const char* content = fs_read(input + 4);
        if (content) {
            print(content);
            print("\n");
        } else {
            print("File not found.\n");
        }
    }
    else if (strncmp(input, "version", 7) == 0){
        print("Your SimonOS version is v0.2.\n");
    }
    else if (strncmp(input, "touch ", 6) == 0) {
        if (fs_create(input + 6)) {
            print("File created.\n");
        } else {
            print("Failed to create file.\n");
        }
    }
    else if (strncmp(input, "rm ", 3) == 0) {
        if (fs_delete(input + 3)) {
            print("File deleted.\n");
        } else {
            print("File not found.\n");
        }
    }
    else {
        print("?: ");
        print(input);
        print("\n");
    }
}

void kernel_main() {
    fs_init();
    print("========SimonOS========\n");
    print("v0.2\n");

    char input[128];
    while (1) {
        print("SimonOS > ");
        read_line(input, sizeof(input));
        process_command(input);
    }
}
