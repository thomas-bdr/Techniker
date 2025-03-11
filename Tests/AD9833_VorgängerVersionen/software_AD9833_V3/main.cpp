#include <iostream>
#include <math.h>
#include <wiringPi.h>
#include <bitset>

#include "AD9833CTL.h"

using namespace std;

void unitytest_class_CTLREG()
{
    CTLREG a;
    a.print();
    a.setFlag(CTLREG::B28);
    a.setFlag(CTLREG::RESET);
    a.isFlagSet(CTLREG::RESET);
    a.print();
    uint16_t word = a.getword();
    std::cout << std::bitset<16>(word) << std::endl;
    a.clearFlag(CTLREG::RESET);
    a.print();
}

class WAVECONFIG
{
private:

static constexpr uint32_t MCLK = 25000000;

static constexpr double TWO_PI = 2 * M_PI;
static constexpr double EPSILON = 1e-6;

static constexpr uint32_t MSB_14{0xFFFF0000};

static constexpr uint32_t LSB_14{0x0000FFFF};

public:
CTLREG ctlReg;
void getFreqStream(uint64_t frequenz)
{
    assert((frequenz < 12500001) && "ERROR: max Output Freq 12,5MHz\n");
    uint32_t word = static_cast<uint32_t>((frequenz*((1<<28)-1))/MCLK);
    std::cout  << "BitStream: " << std::bitset<32>(word)<<std::endl;
}

//Create whole word.
uint32_t calcFreqBit(uint64_t frequenz)
{
    assert((frequenz < 12500001) && "ERROR: max Output Freq 12,5MHz\n");
    return (frequenz*((1<<28)-1))/MCLK;
}
uint16_t calcPhaseBit(double phase = 0)
{
    uint16_t word{0};
    while(phase < 0) phase += TWO_PI;
    if (std::fabs(std::fmod(phase, TWO_PI)) < EPSILON) word = static_cast<uint16_t>(phase/(2.0*M_PI)*4096.0);
    return word;
}

uint32_t confFreqWord(uint32_t freq)
{
    uint16_t controlWord = ctlReg.getword();
    uint32_t word = calcFreqBit(freq);
    if(ctlReg.isFlagSet(ctlReg.RESET))
    {

    }

}


};

int main() 
{
    WAVECONFIG a;
    a.getFreqStream(10500000);


    /*
    AD9833 fgen;
    fgen.SPI_SETUP("/dev/spidev0.0",SPI_MODE_1, O_RDWR);
    if (!fgen.isOpen())
    {
        std::cerr << "SPI kommunikation konnte nicht gestartet werden." << std::endl;
        return 0;
    }

    fgen.initAD9833();

    fgen.setFrequency0(10000000, 0);

    //fgen.setFrequency0(1000, 0);

    //fgen.configureAD9833(TRUE);
    */


    return 0;
}   
