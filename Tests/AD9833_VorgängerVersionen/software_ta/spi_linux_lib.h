#ifndef SPI_LINUX_LIB_H
#define SPI_LINUX_LIB_H

#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <cstring>

class SPI_Device {
private:
    std::string DEVICE;
    int SPI_FD;
    const uint32_t SPI_SPEED = 1000;  // 1 MHz
    uint8_t SPI_MODE;
    uint8_t FLAGE;

public:
    // Konstruktor mit Gerätepfad
    SPI_Device(const std::string& devicePath, const uint8_t& spi_mode, const uint8_t& flag);
    
    // Destruktor zum Schließen des SPI-Handles
    ~SPI_Device();

    // Sendet ein Byte über SPI
    void sendData(const uint16_t& data);

    // Lese ein Byte über SPI
    uint16_t readData();
    
    // Prüft, ob das Gerät erfolgreich geöffnet wurde
    bool isOpen() const;
};

void SPI_test(std::string a);

#endif 