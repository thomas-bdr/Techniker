#include <iostream>
#include <chrono>
#include <vector>

#include "AD9833.h"

using namespace std;

enum class FrequencyPreset
{
    FREQ_1kHz,
    FREQ_10KHZ,
    FREQ_100KHZ,
    FREQ_200KHZ,
    FREQ_300KHZ,
    FREQ_500KHZ,
    FREQ_1MHZ
};

inline double getFrequencyValue(FrequencyPreset preset)
{
    switch (preset)
    {
        case FrequencyPreset::FREQ_1kHz:    return 1000;
        case FrequencyPreset::FREQ_10KHZ:   return 10000;
        case FrequencyPreset::FREQ_100KHZ:  return 100000;
        case FrequencyPreset::FREQ_200KHZ:  return 200000;
        case FrequencyPreset::FREQ_300KHZ:  return 300000;
        case FrequencyPreset::FREQ_500KHZ:  return 500000;
        case FrequencyPreset::FREQ_1MHZ:    return 1000000;
        default:                            return 100;
    }
}

struct DemoCycle
{
    AD9833 AD;

    using clock = std::chrono::high_resolution_clock;

    clock::time_point start;
    clock::time_point end;

    DemoCycle(uint32_t mclk = 19480000): AD(mclk){}
    ~DemoCycle(){}

    void initCycle(uint32_t frequency)
    {
        AD.Initial();
        AD.ApplySignal(SINE_WAVE, REG0, REG1, frequency, 0);
        AD.ApplySignal(SINE_WAVE, REG1, REG0, frequency, 0);
        AD.SetOutputSource(REG0, REG1);
        printOutRealSetup();
        readValueADC();
    }

    void printOutRealSetup(void)
    {
        cout << "Frequenregister0: " << AD.GetActualProgrammedFrequency(REG0) << endl
        << "Phasenregister0: " << AD.GetActualProgrammedPhase(REG0) << endl
        << "Resolution: " << AD.GetResolution() << endl;
    }

    void readValueADC(void)
    {
        start = clock::now();
        for(int i = 1;i < 1000e6; i++){};

        end = clock::now();
        cout << "Dauer Schleife: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }

    void repeateCycle(const double frequency)
    {
        AD.SetFrequency(REG0, frequency);
        printOutRealSetup();
        readValueADC();
    }
};


int main()
{   
    DemoCycle showCase;
    showCase.initCycle(getFrequencyValue(FrequencyPreset::FREQ_1kHz));

    std::vector<FrequencyPreset> presets =
    {
        FrequencyPreset::FREQ_1kHz,
        FrequencyPreset::FREQ_10KHZ,
        FrequencyPreset::FREQ_100KHZ,
        FrequencyPreset::FREQ_200KHZ,
        FrequencyPreset::FREQ_300KHZ,
        FrequencyPreset::FREQ_500KHZ,
        FrequencyPreset::FREQ_1MHZ
    };

    for(const auto& preset: presets)
    {
        double freq = getFrequencyValue(preset);
        showCase.repeateCycle(freq);
    }
    
    return 0;
}
