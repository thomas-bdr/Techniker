#include <iostream>
#include <math.h>
#include <bitset>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

#include "AD9833CTL.h"


AD9833::AD9833()
{
    spi_fd = open(device, O_RDWR);
    if (spi_fd < 0)
    {
        std::cerr << "Fehler: SPI konnte nicht geöffnet werden!" << std::endl;
        exit(1);
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0 ||
        ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||
        ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        std::cerr << "Fehler: SPI-Parameter konnten nicht gesetzt werden!" << std::endl;
        close(spi_fd);
        exit(1);
    }
}
AD9833::~AD9833()
{
    if (spi_fd >= 0)
    {
        close(spi_fd);
    }
}

// ///////////////////////// //
// /////Control Reg Func//// //
// ///////////////////////// //


void AD9833::setFlag(Flags flag)
{
    word |= flag;
}

void AD9833::clearFlag(Flags flag)
{
    word &= ~flag;
}

bool AD9833::isFlagSet(Flags flag)
{
    return (word & flag) !=0;
}
uint16_t AD9833::getword() const
{
    return word;
}
void AD9833::print() const
{
    std::cout << "Control Word: " << std::bitset<16>(word) << std::endl;
}

// ///////////////////////// //
// /////Waveform Generat//// //
// ///////////////////////// //

void AD9833::printConfPhaseWord(double phase, Flags phaseReg)
{
    uint32_t phaseWord = confPhaseWord(phase, phaseReg);
    std::cout << "Configured control register word for Phase: " << std::bitset<16>(phaseWord) << std::endl;
}

void AD9833::printPhaseStream(double phase)
{
    uint16_t pword = calcPhase(phase);
    std::cout << "Bitstream of Phase: "<< std::hex << pword << std::endl;
}

uint16_t AD9833::confPhaseWord(double phase, Flags type)
{
    uint16_t pword = calcPhase(phase);
    if(type == PHASE0)
    {
        pword = pword | PHASE0; 
        pword &= ~0x2000; // delet D13 to make sure Phase0 used
    }
    //bedingung anpassen!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if(type == PHASE1)
    {
        pword = pword | PHASE1;
    }
    else
        std::cerr << "Wrong Phase type" << std::endl;

    return pword;
}

void AD9833::printConfFWord(uint32_t freq, Flags type)
{
    uint32_t freqword = confFWord(freq, type);
    std::cout << "Configured control register word for Frequency: " << std::bitset<32>(freqword) << std::endl;
}

void AD9833::printFreqStream(uint64_t frequency)
{
    uint32_t word = calcFreq(frequency);
    std::cout  << "BitStream: " << std::bitset<32>(word)<<std::endl;
}

// Verwendung von Controll Register integrieren
uint32_t AD9833::confFWord(uint32_t freq, Flags type)
{
    bool freqReg{0};
    if(type == FREQ0) freqReg = 0;
    if(type == FREQ1) freqReg = 1;
    uint16_t controlWord = AD9833::getword();
    uint32_t calcWord = calcFreq(freq);
    uint32_t confiWord{0};
    constexpr uint32_t upperPartCalFreq{0x0FFFC000};
    constexpr uint32_t lowerPartCalFreq{0x3FFF};

    bool B28Flag = AD9833::isFlagSet(AD9833::B28);
    bool HLBFlag = AD9833::isFlagSet(AD9833::HLB);

    if(B28Flag && HLBFlag)
    {
        std::cerr << "ERROR: Controll Register. HLB and B28 set." << std::endl;
        return -1;
    }
    if(B28Flag)
    {
        confiWord = (calcWord >> 14) | (freqReg ? AD9833::FREQ1 : AD9833::FREQ0);
        return confiWord << 16 | (calcWord & lowerPartCalFreq) | (freqReg ? AD9833::FREQ1 : AD9833::FREQ0);
    }
    if((HLBFlag))
    {
        if((calcWord&upperPartCalFreq)!=calcWord)
        {
            std::cerr << "WARNING: frequency word contains MSB\n";
        }
        if((calcWord>>14)>lowerPartCalFreq)
        {
            std::cerr << "WARNING: Calculation error. Frequency word bigger than 14Bit.\n";
        }
        return static_cast<uint16_t>(((calcWord>>14)&lowerPartCalFreq) | (freqReg ? AD9833::FREQ1 : AD9833::FREQ0));
    }
    if((calcWord&upperPartCalFreq)!=calcWord)
    {
        std::cerr << "WARNING: frequency word contains LSB\n";
    }
    if((calcWord)>lowerPartCalFreq)
    {
        std::cerr << "WARNING: Calculation error. Frequency word bigger than 14Bit.\n";
    }
    return static_cast<uint16_t>((calcWord&lowerPartCalFreq) | (freqReg ? AD9833::FREQ1 : AD9833::FREQ0));
}

// ///////////////////////// //
// /////Sequenc function//// //
// ///////////////////////// //  


// ///////////////////////// //
// /////SPI lib function//// //
// ///////////////////////// // 

void AD9833::SPI_sendData16Bit(const uint16_t& data)
{
    std::vector<uint8_t> tx_buffer = 
    {
        static_cast<uint8_t>(data>>8), 
        static_cast<uint8_t>(data& 0xFF)
    };
    std::vector<uint8_t> rx_buffer = {2, 0};

    SPI_sendData(tx_buffer, rx_buffer);
}

void AD9833::SPI_sendData32Bit(const uint32_t& data)
{
    std::vector<uint8_t> tx_buffer = 
    {
        static_cast<uint8_t>(data >>  24),
        static_cast<uint8_t>(data >> 16), 
        static_cast<uint8_t>(data >> 8), 
        static_cast<uint8_t>(data & 0xFF)
    };
    std::vector<uint8_t> rx_buffer = {4, 0};

    SPI_sendData(tx_buffer, rx_buffer);
}


/*

void AD9833::initAD9833()
{
    std::cout << "Sende an SPI-Gerät 1 Chipselect:(CE0, GPIO8, Pin24), MOSI: (GPI10, PIN19)..." << std::endl;
    //Init Sequenz
    sendSPIData(B28 | RESET);
    //setup freq0
    sendSPIData(0x4000);
    sendSPIData(0x7FFF);
    //setup Freq1
    sendSPIData(0x8000);
    sendSPIData(0xBFFF);
    //setup Phase0
    sendSPIData(0xC000);
    //setup Phase1
    sendSPIData(0xE000);
    //reset bit = 0
    sendSPIData(B28);
    std::cout << "Init successfully\n";
}

*/

