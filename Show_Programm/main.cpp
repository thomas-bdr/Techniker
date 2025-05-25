#include <iostream>
#include <chrono>

#include "AD9833.h"

using namespace std;


const double setupFrequency0 = 1000;
const double setupFrequency1 = 10000;
const double setupFrequency2 = 100000;
const double setupFrequency3 = 500000;
const double setupFrequency4 = 1000000;




int main()
{   
    //init
    AD9833 AD(19480000);
    AD.Initial();
    using clock = std::chrono::high_resolution_clock;
    //cycle 0
    AD.ApplySignal(SINE_WAVE, REG0, REG1, setupFrequency0, 0);
    AD.ApplySignal(SINE_WAVE, REG1, REG0, 1000, 0);
    AD.SetOutputSource(REG0, REG1);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    auto start = clock::now();
    for(int i = 1;i < 1000e6; i++){};

    auto end = clock::now();
    cout << "Dauer Schleife 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";

    //cycle 1
    AD.SetFrequency(REG0, setupFrequency1);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    start = clock::now();
    for(int i = 1;i < 1000e6; i++){};

    end = clock::now();
    cout << "Dauer Schleife 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";


    //cycle 2
    AD.SetFrequency(REG0, setupFrequency2);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    start = clock::now();
    for(int i = 1;i < 1000e6; i++){};

    end = clock::now();
    cout << "Dauer Schleife 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";

    
    //cycle 3
    AD.SetFrequency(REG0, setupFrequency3);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    start = clock::now();
    for(int i = 1;i < 1000e6; i++){};

    end = clock::now();
    cout << "Dauer Schleife 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";


    //cycle 4
    AD.SetFrequency(REG0, setupFrequency4);
    cout << "actual Frequency Register: 0" << AD.GetActualProgrammedFrequency(REG0) << endl
    << "actual Phase Register: 1" << AD.GetActualProgrammedPhase(REG1) << endl
    << "Resolution: " << AD.GetResolution() << endl;
    //hier wird dein for-Schleife implementiert
    start = clock::now();
    for(int i = 1;i < 1000e6; i++){};

    end = clock::now();
    cout << "Dauer Schleife 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";

    
    return 0;
}
