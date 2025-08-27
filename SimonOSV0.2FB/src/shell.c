#include "shell.h"
#include <string.h>

static char output[256];

const char* shell(const char* input) {
    static char output[1024];
    
    if (strncmp(input, "help", 4) == 0) {
        strncpy(output, 
            "Available commands:\n"
            " help - Show this help\n"
            " clear - Clear screen\n"
            " echo [text] - Repeat text\n"
            " info - Show system info",
            sizeof(output));
        return output;
    }

    if (strncmp(input, "clear", 5) == 0) {
        return "e\n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n";
    }

    if (strncmp(input, "echo ", 5) == 0) {
        strncpy(output, input + 5, sizeof(output) - 1);
        output[sizeof(output) - 1] = '\0';
        return output;
    }

    if (strncmp(input, "info", 4) == 0) {
        strncpy(output,
            "System Information:\n"
            " OS: MyOS\n"
            " Version: 0.1\n"
            " Memory: 16MB\n"
            " Display: 1024x768",
            sizeof(output));
        return output;
    }

    strncpy(output, "Unknown command. Type 'help' for available\ncommands.", sizeof(output));
    return output;
}