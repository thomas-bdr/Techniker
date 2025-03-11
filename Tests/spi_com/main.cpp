#include <iostream>
#include <pigpio.h>  // Für schnelles GPIO-Schalten
#include <cstdint>

#define GPIO_PIN 4  // BCM GPIO 4 (physischer Pin 7)

// ARM Cycle Counter auslesen (Nanosekunden-genau)
inline uint64_t read_cycle_counter() {
    uint64_t val;
    asm volatile("mrs %0, CNTVCT_EL0" : "=r"(val));  // ARM Performance Counter auslesen
    return val;
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "Fehler: pigpio konnte nicht initialisiert werden!\n";
        return 1;
    }

    gpioSetMode(GPIO_PIN, PI_OUTPUT);
    std::cout << "Erzeuge exakte 1 MHz Rechteckwelle auf GPIO " << GPIO_PIN << "...\n";

    uint64_t start_time = read_cycle_counter();
    const uint64_t cycle_delay = 26; // 500 ns bei ~1.92 GHz CPU-Takt

    while (true) {
        gpioWrite(GPIO_PIN, 1);
        while (read_cycle_counter() - start_time < cycle_delay){};  // 500 ns warten
        start_time = read_cycle_counter();

        gpioWrite(GPIO_PIN, 0);
        while (read_cycle_counter() - start_time < cycle_delay){};  // 500 ns warten
        start_time = read_cycle_counter();
    }

    gpioTerminate();
    return 0;
}