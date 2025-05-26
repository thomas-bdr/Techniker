#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
//----------------------------
#include <fstream>
#include <vector>
#include <cstdint>
#include <thread>

// Datenstruktur, um die Messungsnummer und den 16-Bit Integer-Wert zu speichern
struct Measurement {
    int id;       // Auto inkrementierende Messungsnummer
    uint16_t value;
};

// Vektor zum Speichern der Messungen
std::vector<Measurement> data;

// Zähler für die Messungsnummer, startet bei 0
int measurementCounter{0};


class AD4000 {
private:
    int spi_fd;
    std::string spi_device = "/dev/spidev0.0";  // SPI-Schnittstelle
    int spi_speed = 1000000;                    // 1 MHz
    int spi_mode = SPI_MODE_0;                  // SPI Mode 0 für AD4000
    int bits_per_word = 8;                      // Standard 8 Bit pro Wort

    static constexpr uint8_t readRegisterAccess = 0x54;
    static constexpr uint8_t writeRegisterAccess = 0x14;
    static constexpr uint8_t stateconfig = 0x02;

public:
    AD4000() {

        // SPI Schnitstelle setzen
        spi_fd = open(spi_device.c_str(), O_RDWR);
        if (spi_fd < 0) {
            perror("Fehler beim Öffnen der SPI-Schnittstelle");
            exit(EXIT_FAILURE);
        }

        // SPI Modus setzen
        if (ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode) < 0) {
            perror("Fehler beim Setzen des SPI-Modus");
            exit(EXIT_FAILURE);
        }

        // SPI-Takt setzen
        if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0) {
            perror("Fehler beim Setzen der SPI-Geschwindigkeit");
            exit(EXIT_FAILURE);
        }
    }

    ~AD4000() {
        close(spi_fd);
    }

    uint16_t spi_registerReadAD4000() {
        uint8_t txBuffer[2] = {readRegisterAccess, 0xFF};                           // Senden an Slave die 8-Bit Register Access Command zum lesen (0x54)
        uint8_t rxBuffer[2] = {0x00, 0x00};                     // Erhalt der Daten vom Slave, MSB ==> ADC-Value (!!! nicht der vollständige ADC-Wert !!!), LSB ==> Register Daten
        spi_transfer(txBuffer, rxBuffer, 2);

        return rxBuffer[1] & 0x1E;
    }

    uint16_t spi_registerWriteAD4000() {
        uint8_t txBuffer[2] = {writeRegisterAccess, stateconfig};   // Senden an Slave Register Access Comand zum schreiben (0x14) und die Daten für die Statusregister (0x12)
        uint8_t rxBuffer[2] = {0x00, 0x00};                         // Erhalt der ADC-Value Werte
        spi_transfer(txBuffer, rxBuffer, 2);

        uint16_t measurement = (rxBuffer[0] << 8) | rxBuffer[1];
        return measurement;
    }

    uint16_t spi_readADCvalue() {
        uint8_t txBuffer[2] = {0xFF, 0xFF};
        uint8_t rxBuffer[2] = {0x00, 0x00};
        spi_transfer(txBuffer, rxBuffer, 2);

        uint16_t measurement = (rxBuffer[0] << 8) | rxBuffer[1];
        return measurement;
    }


    void spi_transfer(uint8_t* tx, uint8_t* rx, uint8_t length) {
        struct spi_ioc_transfer spi_transfer = {};
        spi_transfer.tx_buf = (unsigned long)tx;
        spi_transfer.rx_buf = (unsigned long)rx;
        spi_transfer.len = length;
        spi_transfer.speed_hz = spi_speed;
        spi_transfer.bits_per_word = bits_per_word;
        spi_transfer.cs_change = 0;
        //spi_transfer.delay_usecs = 10000; // falls nötig wegen t_quiet2

        if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 0) {
            perror("SPI-Datenübertragung fehlgeschlagen");
            exit(EXIT_FAILURE);
        }
    }
};

int main()
{
    AD4000 adc4000;

    std::cout << adc4000.spi_registerReadAD4000() << std::endl;  // Diese Funktion macht genau das was sie soll

    //std::cout << adc4000.spi_registerWriteAD4000() << std::endl; // Diese Funktion macht genau das was sie soll

    //std::cout << adc4000.spi_readADCvalue() << std::endl;
/*
    for (int i = 0; i < 1000000; i++){

        uint16_t value = adc4000.spi_readADCvalue();

        // Messung in den Vektor speichern mit der aktuellen Messungsnummer
        data.push_back({measurementCounter++, value});

        // Ausgabe der aktuellen Messung auf der Konsole
        //std::cout << "Messung " << measurementCounter - 1 << " - Wert: " << value << "\n";

        // Optional: Kurze Pause, um den Durchlauf zu verlangsamen
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
*/
/*
    // Schreiben der gespeicherten Daten in eine Textdatei
    std::ofstream outFile("data.txt");
    if (outFile.is_open()) {
        for (const auto& measurement : data) {
            outFile << measurement.id << " - Wert: " << measurement.value << "\n";
        }
        outFile.close();
        std::cout << "\nDaten wurden in data.txt gespeichert.\n";
    } else {
        std::cerr << "\nFehler beim Öffnen der Datei data.txt\n";
    }
*/


    return 0;
}
