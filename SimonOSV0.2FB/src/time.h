#ifndef _SOFTWARE_TIME_H_
#define _SOFTWARE_TIME_H_

#include <stdint.h>

// Initialize with estimated CPU frequency
void time_init(uint32_t estimated_cpu_hz);

// Sleep functions (busy-wait)
void sleep_ms(uint32_t milliseconds);
void sleep_us(uint32_t microseconds);

// Time tracking (since init)
uint32_t millis(void);
uint32_t micros(void);

// Delay aliases
#define delay_ms sleep_ms
#define delay_us sleep_us

#endif // _SOFTWARE_TIME_H_