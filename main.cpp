#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>

class AD4000 {
private:
    int spi_fd;
    std::string spi_device = "/dev/spidev0.0"; // SPI-Schnittstelle
    int spi_speed = 1000000; // 1 MHz
    int spi_mode = SPI_MODE_1; // SPI Mode 0 für AD4000
    int bits_per_word = 8; // Standard 8 Bit pro Wort

    static constexpr uint8_t readRegisterAccess = 0x54;
    static constexpr uint16_t writeRegisterAccess = 0x1400; //letzte 8 LSB werden individuel configuriert status bits
    static constexpr uint8_t stateconfig = 0x12; //10010

public:
    AD4000() {

        spi_fd = open(spi_device.c_str(), O_RDWR);
        if (spi_fd < 0) {
            perror("Fehler beim Öffnen der SPI-Schnittstelle");
            exit(EXIT_FAILURE);
        }

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


    uint16_t spi_com(const uint16_t &data) {
        uint8_t txBuffer[2] = {
            0x00,
            0x00};
        uint8_t rxBuffer[2] = {0x00, 0x00};
        spi_transfer(txBuffer, rxBuffer, 2);

        uint16_t measurement = (rxBuffer[0] << 8) | rxBuffer[1];
        return measurement;
    }

    uint16_t spi_registerReadAD4000(const uint8_t &data) {
        uint8_t txBuffer[1] = {
            0x54};
        uint8_t rxBuffer[2] = {0x00, 0x00};
        spi_transfer(txBuffer, rxBuffer, 2);

        uint16_t measurement = (rxBuffer[0] << 8) | rxBuffer[1];        //
        return measurement;
    }

    uint16_t spi_registerWriteAD4000(const uint8_t &data, uint8_t &stateconfig) {

        uint8_t txBuffer[1] = {
            0x54,
            stateconfig};
        uint8_t rxBuffer[2] = {0x00, 0x00};
        spi_transfer(txBuffer, rxBuffer, 2);

        uint16_t measurement = (rxBuffer[0] << 8) | rxBuffer[1];        //
        return measurement;
    }

    uint32_t spi_readADCvalue(const uint8_t &data) {
        uint8_t txBuffer[1] = {
            0x54};
        uint8_t rxBuffer[2] = {0x00, 0x00};
        spi_transfer(txBuffer, rxBuffer, 2);

        uint16_t measurement = (rxBuffer[0] << 8) | rxBuffer[1];        //
        return measurement;
    }



    void spi_testMosi() {
        uint8_t testBuffer[2] = {0xAA, 0x55}; // Testdaten für MOSI
        uint8_t rxBuffer[2] = {0x00, 0x00};
        while (true) {
            spi_transfer(testBuffer, rxBuffer, 2);
            std::cout << "Gesendete MOSI-Daten: 0x" << std::hex << (int)testBuffer[0] << " 0x" << (int)testBuffer[1] << std::dec << std::endl;
            usleep(10000); // 100ms Pause
        }
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

int main() {
    AD4000 adc;

    std::cout << "Starte SPI-MOSI-Test..." << std::endl;
    adc.spi_testMosi(); // Dauerschleife zum Testen des MOSI-Signals



    return 0;
}
