#include <iostream>
#include <math.h>

#include "AD9833CTL.h"

CTLREG::CTLREG():word(0)
{}

void CTLREG::setFlag(Flags flag)
{
    word |= flag;
}

void CTLREG::clearFlag(Flags flag)
{
    word &= ~flag;
}

bool CTLREG::isFlagSet(Flags flag)
{
    return (word & flag) !=0;
}
uint16_t CTLREG::getword() const
{
    return word;
}
void CTLREG::print() const
{
    std::cout << "Control Word: " << std::hex << word << std::endl;
}


/*

AD9833::AD9833()
{
}

void AD9833::setAD9833(uint32_t frequence, uint16_t phase)
{
    /*  uint16_t setphase = configurePhase(phase);
    sendSPIData(setphase);
    uint32_t setfreq = configureFreq(frequence);
    sendSPIData(setfreq);
}



void AD9833::SPI_SETUP(const std::string& devicePath, const uint8_t& spi_mode, const uint8_t& flage)
{
    DEVICE = devicePath;
    SPI_FD = -1;
    SPI_MODE = spi_mode;
    FLAGE = flage;
    //Build Initialization
    std::cout << "AD9833 und SPI COM was initialise" << std::endl;


    SPI_FD = open(DEVICE.c_str(), FLAGE);
    if (SPI_FD < 0)
    {
        std::cerr << "ERROR: Couldn't open SPI-Com: " << DEVICE << std::endl;
        return;
    }

    if (ioctl(SPI_FD, SPI_IOC_WR_MODE, &SPI_MODE) < 0)
    {
        std::cerr << "ERROR: Couldn't setup SPI Mode" << std::endl;
        close(SPI_FD);
        SPI_FD = -1;
        return;
    }

    if (ioctl(SPI_FD, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED) < 0)
    {
        std::cerr << "ERROR: Couldn't setup SPI Speed!" << std::endl;
        close(SPI_FD);
        SPI_FD = -1;
        return;
    }
}

bool AD9833::isOpen() const
{
    return SPI_FD >= 0;
}

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

//SetupBits HLB or B28
//B28 sends 

void AD9833::setFrequency0(uint32_t frequency,uint32_t phase)
{
    setupBitStream32(FREQ0, frequency, phase);
}
void AD9833::setFrequency1(uint32_t frequency, uint32_t phase)
{
    setupBitStream32(FREQ1, frequency, phase);
}

//Controllregister 
/*
void AD9833::sendControllRegister(uint16_t controlWord)
{
    sendSPIData(controlWord);
}



//dekonstructor
AD9833::~AD9833()
{
    std::cout << "AD9833 end" << std::endl;
    if (SPI_FD >= 0)
    {
        close(SPI_FD);
    }
}

*/


/**/

