#include <iostream>

#include "AD9833.h"

using namespace std;


const double setupFrequency1 1000;
const double setupFrequency0 10000;
const double setupFrequency1 100000;
const double setupFrequency1 500000;
const double setupFrequency1 1000000;


int main()
{   
    //init
    AD9833 AD;
    AD.Initial();
    //cycle 0
    AD.ApplySignal(SINE_WAVE, REG1, REG1, setupFrequency, 0);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    //for(){}

    //cycle 1
    AD.ApplySignal(SINE_WAVE, REG1, REG1, setupFrequency, 0);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    //for(){}

    //cycle 2
    AD.ApplySignal(SINE_WAVE, REG1, REG1, setupFrequency, 0);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    //for(){}

    
    //cycle 3
    AD.ApplySignal(SINE_WAVE, REG1, REG1, setupFrequency, 0);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    //for(){}


    //cycle 4
    AD.ApplySignal(SINE_WAVE, REG1, REG1, setupFrequency, 0);
    AD.ApplySignal(SINE_WAVE, REG0, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    //for(){}

    
    return 0;
}
