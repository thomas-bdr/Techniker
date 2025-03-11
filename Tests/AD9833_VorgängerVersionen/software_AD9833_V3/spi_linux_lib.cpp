#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <cstring>

#include "AD9833_lib.h"
#include "spi_linux_lib.h"


SPI_Device::SPI_Device(const std::string& devicePath, const uint8_t& spi_mode, const uint8_t& flage) : DEVICE(devicePath), SPI_FD(-1), SPI_MODE(spi_mode), FLAGE(flage)
{
    //fd = file descriptor = Addresse des SPI Buses
    SPI_FD = open(DEVICE.c_str(), FLAGE);
    if (SPI_FD < 0)
    {
        std::cerr << "Fehler: Konnte SPI nicht öffnen: " << DEVICE << std::endl;
        return;
    }

    // SPI-Modus setzen
    if (ioctl(SPI_FD, SPI_IOC_WR_MODE, &SPI_MODE) < 0)
    {
        std::cerr << "Fehler: Konnte SPI-Modus nicht setzen!" << std::endl;
        close(SPI_FD);
        SPI_FD = -1;
        return;
    }

    // SPI-Taktrate setzen
    if (ioctl(SPI_FD, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED) < 0)
    {
        std::cerr << "Fehler: Konnte SPI-Speed nicht setzen!" << std::endl;
        close(SPI_FD);
        SPI_FD = -1;
        return;
    }
}

SPI_Device::~SPI_Device()
{
    if (SPI_FD >= 0)
    {
        close(SPI_FD);
    }
}

//sendet ein 8 Bit wort
void SPI_Device::sendData(const uint16_t& data)
{
    if (SPI_FD < 0)
    {
        std::cerr << "Fehler: SPI ist nicht geöffnet!" << std::endl;
        return;
    }
    uint16_t tx_buffer[1] = {data};
    uint16_t rx_buffer[1] = {0x0000};
    //initialisierung
    struct spi_ioc_transfer spi_transfer = {};
    spi_transfer.tx_buf = reinterpret_cast<unsigned long>(tx_buffer);
    spi_transfer.rx_buf = reinterpret_cast<unsigned long>(rx_buffer);
    spi_transfer.len = sizeof(tx_buffer);
    spi_transfer.speed_hz = SPI_SPEED;
    spi_transfer.bits_per_word = 8;
    spi_transfer.delay_usecs = 0;
    if (ioctl(SPI_FD, SPI_IOC_MESSAGE(1), &spi_transfer) < 0)
    {
        std::cerr << "Fehler: Konnte SPI-Daten nicht senden!" << std::endl;
        return;
    }
    std::cout << "Gesendet: 0x" << std::hex << (int)tx_buffer[0]
              << " | Empfangen: 0x" << (int)rx_buffer[0] << std::endl;
}

uint16_t SPI_Device::readData()
{
    if (SPI_FD < 0) 
    {
        std::cerr << "Fehler: SPI ist nicht geöffnet!" << std::endl;
        return 0;
    }
    uint16_t tx_buffer[1] = {0x0000};
    uint16_t rx_buffer[1] = {0x0000};
    struct spi_ioc_transfer spi_transfer = {};
    spi_transfer.tx_buf = reinterpret_cast<unsigned long>(tx_buffer);
    spi_transfer.rx_buf = reinterpret_cast<unsigned long>(rx_buffer);
    spi_transfer.len = sizeof(rx_buffer);              
    // Nur lesen, keine echten Daten senden
    if (ioctl(SPI_FD, SPI_IOC_MESSAGE(1), &spi_transfer) < 0)
    {
        std::cerr << "Fehler: Konnte SPI-Daten nicht lesen!" << std::endl;
        return 0;
    }
    std::cout << "Empfangen: 0x" << std::hex << rx_buffer[0] << std::endl;
    return rx_buffer[0];
}

bool SPI_Device::isOpen() const
{
    return SPI_FD >= 0;
}

void SPI_test(std::string a)
{
    std::cout << a;
}

