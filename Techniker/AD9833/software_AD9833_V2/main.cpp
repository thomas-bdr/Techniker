#include <iostream>
#include <math.h>
#include <bitset>
//#include <array>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <unistd.h>

#include "AD9833CTL.h"

using namespace std;

void unitytest_class_CTLREG()
{
    AD9833 a;
    a.print();
    a.setFlag(AD9833::B28);
    a.setFlag(AD9833::RESET);
    a.isFlagSet(AD9833::RESET);
    a.print();
    uint16_t word = a.getword();
    std::cout << std::bitset<16>(word) << std::endl;
    a.clearFlag(AD9833::RESET);
    a.print();
}

void unityTest_class_WAVECONFIG()
{
    AD9833 a;
    a.printFreqStream(12500000);
    a.printPhaseStream(6);
    a.printPhaseStream((2.0 * M_PI));
    a.printPhaseStream((6.282));

    a.printConfPhaseWord(6.282, AD9833::PHASE1);
    a.printConfPhaseWord(6.282, AD9833::PHASE0);

    a.setFlag(AD9833::B28);

    a.printConfFWord(12500000, AD9833::FREQ1);
    a.printConfFWord(12500000, AD9833::FREQ0);

    a.clearFlag(AD9833::B28);
    a.setFlag(AD9833::HLB);

    a.printConfFWord(12500000, AD9833::FREQ1);
    a.printConfFWord(12500000, AD9833::FREQ0);

    a.clearFlag(AD9833::HLB);
    a.printConfFWord(12500000, AD9833::FREQ1);
    a.printConfFWord(12500000, AD9833::FREQ0);
    return;
}

void unityTest_class_SPI()
{
    AD9833 spi;  

    uint16_t data = 0x0000;  
    uint16_t data1 = 0x0000; 

    while (data <= 0xFFF0)
    {
        data1 = data;
        spi.SPI_sendData16Bit(data);
        usleep(50000);  
        data+=10;
        if(data1!=data)
            std::cout << data << std::endl;
    }
    while (data <= 0xFFFFFFF0)
    {
        data1 = data;
        spi.SPI_sendData32Bit(data);
        usleep(50000);  
        data+=10;
        if(data1!=data)
            std::cout << data << std::endl;
    }
}

class SEQ : AD9833
{
private:


public:
    void initialization()
    {
        setFlag(AD9833::RESET);
        setFlag(AD9833::B28);
        print();
        SPI_sendData16Bit(getword());

        SPI_sendData32Bit(confFWord(2000,AD9833::FREQ0));
        SPI_sendData32Bit(confFWord(4000,AD9833::FREQ1));

        SPI_sendData16Bit(confPhaseWord(0,AD9833::PHASE0));
        SPI_sendData16Bit(confPhaseWord(M_PI,AD9833::PHASE1));

        clearFlag(AD9833::RESET);
        print();
        SPI_sendData16Bit(getword());
    }

    


};

int main()
{   
    //unitytest_class_CTLREG();
    unityTest_class_WAVECONFIG();
    //unityTest_class_SPI();

    SEQ sed;
    sed.initialization();
    return 0;
}
