#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <iostream>
#include <stdio.h>
#include <cstdint>

// SPI-Konfiguration
#define SPI_CHANNEL 0        // SPI-Kanal 0
#define SPI_SPEED 1000000    // SPI-Takt 1 MHz

#define CNV_PIN 18

// SPI initialisieren
bool initializeSPI()
{
    if (wiringPiSetupGpio() == -1)
    {
        std::cerr << "Fehler beim Initialisieren von WiringPi." << std::endl;
        return false;
    }

    if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)
    {
        std::cerr << "Fehler beim Initialisieren des SPI-Busses." << std::endl;
        return false;
    }

    return true;
}

// ADC-Daten auslesen
uint16_t readADC()
{
    // Puffer für die SPI-Datenübertragung
    uint8_t buffer[2] = {0x00, 0x00};

    delayMicroseconds(100);

    // SPI-Datenübertragung
    wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2);

    // 16-Bit-Daten aus dem Puffer zusammensetzen
    uint16_t adcValue = (buffer[0] << 8) | buffer[1];

    return adcValue;

    delayMicroseconds(100);
}

// CNV Signal erzeugen
void pwm(int divider, int range, int value)
{
    // Pin als PWM Ausgang definieren
    pinMode(CNV_PIN, PWM_OUTPUT);

    // PWM Betriebsmodus einstellen (MS ==> Markt-Space Mode)
    pwmSetMode(PWM_MODE_MS);

    // Divider einstellen (Teilt die Grundfrequenz 19,2MHz)
    pwmSetClock(divider);
	
    // Stellt die Genauigkeit des Duty Cycle ein
    pwmSetRange(range);
	
    // Startet das PWM Signal
    pwmWrite(CNV_PIN, value);
    std::cout << "PWM gestartet" << std::endl;
}

int main()
{
	 //SPI initialisieren
    if (!initializeSPI()) return -1;

	// PWM reseten
    pwm(0, 3034, 960);

    delay(1000);
	
	// PWM starten mit einer Frequenz von 3,164kHz und 31,64% Duty Cycle
    pwm(2, 3034, 960);

	// AD Wandler in Dauerschleife auslesen und die Werte auf die Konsole ausgeben
    while (true)
    {
        uint16_t adcValue = readADC();
        std::cout << "ADC-Wert: " << adcValue << std::endl;
    }

    return 0;
}
