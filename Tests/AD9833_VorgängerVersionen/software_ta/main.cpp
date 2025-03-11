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

void unityTest_class_WAVECONFIG()
{
    CTLREG clt;
    WAVECONFIG a(clt);
    a.printFreqStream(12500000);
    a.printPhaseStream(6);
    a.printPhaseStream((2.0 * M_PI));
    a.printPhaseStream((6.282));

    a.clt.setFlag(a.clt.B28);

    a.printConfFWord(12500000, 1);
    a.printConfFWord(12500000, 0);

    a.clt.clearFlag(a.clt.B28);
    a.clt.setFlag(a.clt.HLB);

    a.printConfFWord(12500000, 1);
    a.printConfFWord(12500000, 0);

    a.clt.clearFlag(a.clt.HLB);
    a.printConfFWord(12500000, 1);
    a.printConfFWord(12500000, 0);
    
}

class SEQAD9833
{
private:

public:
    WAVECONFIG& seq;
    SEQAD9833(WAVECONFIG& seq);

    ~SEQAD9833();



};

SEQAD9833::SEQAD9833(WAVECONFIG& seq):seq(seq)
{

}

SEQAD9833::~SEQAD9833()
{

}



int main() 
{
    CTLREG ctlreg;
    WAVECONFIG wave(ctlreg);
    SEQAD9833 genSequence(wave);

    //Unnötig Modularisierung -> alles in eine Klasse packen

   genSequence.seq.clt.setFlag(genSequence.seq.clt.B28);
    





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
