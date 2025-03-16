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

        SPI_sendData32Bit(confFWord(0,AD9833::FREQ0));
        SPI_sendData32Bit(confFWord(0,AD9833::FREQ1));

        SPI_sendData16Bit(confPhaseWord(0,AD9833::PHASE0));
        SPI_sendData16Bit(confPhaseWord(M_PI,AD9833::PHASE1));

        clearFlag(AD9833::RESET);
        print();
        SPI_sendData16Bit(getword());
    }

    void init()
    {
        AD9833 manual;
        manual.setFlag(manual.RESET);
        manual.print();
        manual.SPI_sendData16Bit(manual.getword());
        //usleep(5000);
        manual.SPI_sendData16Bit(0);
    
        manual.setFlag(manual.B28);
        manual.print();
        
        //manual.printConfFWord(0,manual.FREQ0);
        manual.SPI_sendData32Bit(0x40FF4F00);
        //manual.printConfFWord(0,manual.FREQ1);
        manual.SPI_sendData32Bit(0x8000800F);
    
        manual.SPI_sendData16Bit(0xC000);
        manual.SPI_sendData16Bit(0xE000);
    
        //manual.setFlag(manual.FSELECT);
        //manual.setFlag(manual.PSELECT);
        manual.clearFlag(manual.RESET);
        manual.print();
        manual.SPI_sendData16Bit(manual.getword());
    }

    


};

int main()
{   
    //unitytest_class_CTLREG();
    //unityTest_class_WAVECONFIG();
    //unityTest_class_SPI();
    AD9833 manual;
    
    //SEQ sed;
    //sed.initialization();
    //usleep(5000);
    manual.SPI_sendData16Bit(0);

    manual.setFlag(manual.B28);
    manual.print();
    
    //manual.printConfFWord(0,manual.FREQ0);
    manual.SPI_sendData32Bit(0x40FF4F00);
    //manual.printConfFWord(0,manual.FREQ1);
    manual.SPI_sendData32Bit(0x8000800F);

    manual.SPI_sendData16Bit(0xC000);
    manual.SPI_sendData16Bit(0xE000);

    //manual.setFlag(manual.FSELECT);
    //manual.setFlag(manual.PSELECT);
    manual.clearFlag(manual.RESET);
    manual.print();
    manual.SPI_sendData16Bit(manual.getword());




    
    return 0;
}
