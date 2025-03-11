#include <iostream>
#include <math.h>

#include "AD9833_lib.h"
#include "spi_linux_lib.h"

AD9833::AD9833()
{
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

void AD9833::setFlag(ControlFlags flag)
{
    ctlReg |= static_cast<uint16_t>(flag);
}
void AD9833::clearFlag(ControlFlags flag)
{
    ctlReg &= static_cast<uint16_t>(flag);
}
uint16_t AD9833::getControlWord() const
{
    return ctlReg;
}
void AD9833::sendControllRegister(uint16_t controlWord)
{
    sendSPIData(controlWord);
}
void configureAD9833(bool reset=0, bool b28=0, bool hlb=0, 
    bool fselect=0,bool pselect=0, bool sleep1=0, bool sleep12=0, 
    bool opbiten=0, bool div2=0, bool mode=0)
{
    AD9833 ctl;
    if(reset)ctl.setFlag(ControlFlags::RESET);
    if(b28)ctl.setFlag(ControlFlags::B28);
    if(hlb)ctl.setFlag(ControlFlags::HLB);
    if(fselect)ctl.setFlag(ControlFlags::FSELECT);
    if(pselect)ctl.setFlag(ControlFlags::PSELECT);
    if(sleep1)ctl.setFlag(ControlFlags::SLEEP1);
    if(sleep12)ctl.setFlag(ControlFlags::SLEEP12);
    if(opbiten)ctl.setFlag(ControlFlags::OPBITEN);
    if(div2)ctl.setFlag(ControlFlags::DIV2);
    if(mode)ctl.setFlag(ControlFlags::MODE);
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




/**/

