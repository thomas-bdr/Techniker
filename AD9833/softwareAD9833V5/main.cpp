#include <iostream>

#include "AD9833.h"

using namespace std;

void unitest_incrementgeber()
{

    AD9833 AD;


    AD.Initial();

    AD.EnableOutput(true);
    AD.EnableOutput(false);
    
    float startHz = 1000, stopHz = 5000, incHz = 1, sweepTimeSec = 5.0;

    AD.ApplySignal(SINE_WAVE, REG0, REG0, 10, 0);

    //loop
    while(1)
    {
        AD.IncrementFrequency(REG0, startHz - incHz);
        for(float i = startHz; i <= stopHz; i+= incHz)
        {
            AD.IncrementFrequency(REG1, incHz);
        }
    }

}

void unitest02_SetFrequency()
{
    AD9833 AD;
    AD.Initial();
    AD.ApplySignal(SINE_WAVE, REG1, REG1, 10000, 0);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG1, REG1);
}

void unitest03_SetWaveform()
{
    AD9833 AD;
    AD.Initial();
    AD.ApplySignal(TRIANLGE_WAVE, REG1, REG1, 10000, 0);
    AD.ApplySignal(HALF_SQUARE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);
}

void unitest03_Disabledfunction()
{
    AD9833 AD;
    AD.Initial();
    AD.ApplySignal(TRIANLGE_WAVE, REG1, REG1, 10000, 0);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);
    AD.DisableInternalClock(true);
    AD.ApplySignal(HALF_SQUARE_WAVE, REG0, REG0, 1e6, 0);
    AD.DisableInternalClock(false);
    AD.DisableDAC(true);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1e5, 0);
}

void unitest04_ActualValues()
{
    AD9833 AD;
    AD.Initial();
    AD.ApplySignal(TRIANLGE_WAVE, REG1, REG1, 10000, 66);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);

    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
         << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
         << "Resolution: " << AD.GetResolution() << endl;
}

int main()
{   
    AD9833 AD;
    AD.Initial();
    AD.ApplySignal(TRIANLGE_WAVE, REG1, REG1, 10000, 66);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);

    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    
    return 0;
}
