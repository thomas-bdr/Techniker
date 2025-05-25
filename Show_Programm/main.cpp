#include <iostream>
#include <chrono>

#include "AD9833.h"

using namespace std;

struct DemoCycle
{
    const double setupFrequency0 = 1000;
    const double setupFrequency1 = 10000;
    const double setupFrequency2 = 100000;
    const double setupFrequency3 = 500000;
    const double setupFrequency4 = 1000000;

    auto start = clock::now();
    auto end = clock::now();

    void initCycle(const double mclk = 19480000)
    {
        AD9833 AD(mclk);
        AD.Initial();
        using clock = std::chrono::high_resolution_clock;
        //cycle 0
        AD.ApplySignal(SINE_WAVE, REG0, REG1, setupFrequency0, 0);
        AD.ApplySignal(SINE_WAVE, REG1, REG0, 1000, 0);
        AD.SetOutputSource(REG0, REG1);
        printOutRealSetup();
        readValueADC();
    }

    void printOutRealSetup()
    {
        cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
        << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
        << "Resolution: " << AD.GetResolution() << endl;
    }

    void readValueADC()
    {
        start = clock::now();
        for(int i = 1;i < 1000e6; i++){};

        end = clock::now();
        cout << "Dauer Schleife 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }

    void repeateCycle(const double frequency)
    {
        AD.SetFrequency(REG0, setupFrequency1);
        printOutRealSetup();
        readValueADC();
    }
};


int main()
{   
    DemoCycle showCase;
    showCase.initCycle();

    showCase.repeateCycle(showCase.setupFrequency0);

    showCase.repeateCycle(showCase.setupFrequency1);

    showCase.repeateCycle(showCase.setupFrequency2);

    showCase.repeateCycle(showCase.setupFrequency3);

    showCase.repeateCycle(showCase.setupFrequency4);
    
    return 0;
}
