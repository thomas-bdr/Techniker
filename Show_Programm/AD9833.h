#ifndef AD9833_H
#define AD9833_H

#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <array>
#include <vector>

using namespace std;

#define BITS_PER_DEG        11.37777777777778
#define B28_Size            268435456L

#define PHASE_WRITE_CMD     0xC000
#define PHASE1_WRITE_REG    0x2000
#define FREQ0_WRITE_REG     0x4000
#define FREQ1_WRITE_REG     0x8000

#define RESET_CMD           0x0100
#define SLEEP_MODE          0x00C0
#define DISABLE_DAC         0x0040
#define DISABLE_INT_CLOCK   0x0080
#define PHASE1_OUTPUT_REG   0x0400
#define FREQ1_OUTPUT_REG    0x0800

#define RESERVED_BITS_0     0x0215


typedef enum {  SINE_WAVE        = 0x2000, 
                TRIANLGE_WAVE    = 0x2002, 
                SQUARE_WAVE      = 0x2028, 
                HALF_SQUARE_WAVE = 0x2020} Waveform;

typedef enum {  REG0, 
                REG1, 
                Same_As_REG0} Registers;

class AD9833
{
public:
    AD9833  (uint32_t mclk = 25000000);

    ~AD9833 ();

    void        Initial(void);

    void        Reset(void);

    void        ApplySignal(Waveform wavetype, Registers freqReg, Registers phaseReg, double frequencyInHz, double phaseInDeg);

    void        IncrementFrequency(Registers Reg, double freqInHz);

    void        IncrementPhase(Registers Reg, double phaseInDeg);

    void        SetFrequency(Registers Reg, double frequency);

    void        SetPhase(Registers phaseReg, double phaseInDeg);

    void        SetWaveform(Registers freqReg, Waveform wavetype);

    void        SetOutputSource(Registers freqReg, Registers phaseReg);

    void        EnableOutput(bool enable);
                //disable internal CLK, DAC = Output disabled
    void        SleepMode(bool enable);
                //disable DAC, reduce current consumption by Squarewave
    void        DisableDAC(bool enable);
                //disable internal CLK = OUTPUT disabled
    void        DisableInternalClock(bool enable);

    double       GetActualProgrammedFrequency(Registers reg);

    double       GetActualProgrammedPhase(Registers reg);

    double       GetResolution(void);


 
private:
    void        WriteRegister(const uint16_t &data);

    void        WriteControlRegister(void);

    void        DataTransmission(const std::vector<uint8_t> & txBuffer);



    uint16_t    waveForm0, waveForm1, CRTLREG;
    uint8_t     outputEnable, DacDisabled, IntClkDisabled;
    uint32_t    refFrequency;
    volatile double      frequency0, frequency1;
    volatile double      phase0, phase1;
    Registers   activeFreq, activePhase;

    uint8_t     mode, spiBitSize;
    uint32_t    sclkSpeed;
    int         fileDescriptor;
};

#endif