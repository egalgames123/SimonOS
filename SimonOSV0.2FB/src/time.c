#include "time.h"
#include <stdint.h>

// Internal state
static volatile uint32_t milliseconds_count = 0;
static volatile uint32_t microseconds_count = 0;
static uint32_t cycles_per_us = 1;
static uint32_t cycles_per_ms = 1000;

// Calibration values (adjust based on your compiler optimization)
#define DEFAULT_CYCLES_PER_US 8  // Conservative estimate for most CPUs
#define DEFAULT_CYCLES_PER_MS (DEFAULT_CYCLES_PER_US * 1000)

void time_init(uint32_t estimated_cpu_hz) {
    // Calculate rough cycles per microsecond
    if (estimated_cpu_hz > 0) {
        cycles_per_us = estimated_cpu_hz / 1000000;
        cycles_per_ms = cycles_per_us * 1000;
    } else {
        // Fallback to conservative defaults
        cycles_per_us = DEFAULT_CYCLES_PER_US;
        cycles_per_ms = DEFAULT_CYCLES_PER_MS;
    }
}

// Busy-wait for microseconds
void sleep_us(uint32_t microseconds) {
    for (uint32_t i = 0; i < microseconds * cycles_per_us; i++) {
        __asm__ volatile ("nop");  // No-operation to prevent optimization
    }
    microseconds_count += microseconds;
    if (microseconds_count >= 1000) {
        milliseconds_count += microseconds_count / 1000;
        microseconds_count %= 1000;
    }
}

// Busy-wait for milliseconds
void sleep_ms(uint32_t milliseconds) {
    for (uint32_t ms = 0; ms < milliseconds; ms++) {
        sleep_us(1000);  // 1000us = 1ms
    }
}

// Get milliseconds since init
uint32_t millis(void) {
    return milliseconds_count;
}

// Get microseconds since init
uint32_t micros(void) {
    return microseconds_count + (milliseconds_count * 1000);
}