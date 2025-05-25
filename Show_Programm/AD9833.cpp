#include <iostream>
#include <math.h>
#include <bitset>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

#include <iomanip>

#include "AD9833.h"


//public
    AD9833::AD9833  (uint32_t mclk)
    {
        //AD9833 variables
        refFrequency = mclk;
        outputEnable = false;
        IntClkDisabled = false;
        DacDisabled = false;
        waveForm0 = waveForm1 = SINE_WAVE;
        frequency0 = frequency1 = 1000;
        phase0 = phase1 = 0.0;
        activeFreq = REG0;
        activePhase = REG0;
        CRTLREG = 0x0;
        //actualWave = 0x0;

        //SPI variables
        const char *device = "/dev/spidev0.1";
        fileDescriptor = -1;
        mode = SPI_MODE_2;
        spiBitSize = 8;
        sclkSpeed = 4e4;

        fileDescriptor = open(device, O_RDWR);
        if (fileDescriptor < 0)
        {
            std::cerr << "Fehler: SPI konnte nicht geöffnet werden!" << std::endl;
            exit(1);
        }
    
        if (ioctl(fileDescriptor, SPI_IOC_WR_MODE, &mode) < 0 ||
            ioctl(fileDescriptor, SPI_IOC_WR_BITS_PER_WORD, &spiBitSize) < 0 ||
            ioctl(fileDescriptor, SPI_IOC_WR_MAX_SPEED_HZ, &sclkSpeed) < 0)
        {
            std::cerr << "Fehler: SPI-Parameter konnten nicht gesetzt werden!" << std::endl;
            close(fileDescriptor);
            exit(1);
        }

    }

    AD9833::~AD9833 ()
    {
        if (fileDescriptor >= 0)
        {
            close(fileDescriptor);
        }
        
    }

    void AD9833::Initial(void)
    {
        Reset();
        EnableOutput(true);

    }

    void AD9833::Reset(void)
    {
        WriteRegister(RESET_CMD);

    }

    void AD9833::ApplySignal(Waveform wavetype, Registers freqReg,
        Registers phaseReg, double frequencyInHz, double phaseInDeg)
    {
        SetFrequency(freqReg, frequencyInHz);
        SetPhase(phaseReg, phaseInDeg);
        SetWaveform(freqReg, wavetype);
        SetOutputSource(freqReg, phaseReg);

    }

    void AD9833::IncrementFrequency(Registers Reg, double change)
    {
        float frequency = (Reg == REG0) ? frequency0 : frequency1;
        SetFrequency(Reg, change);
        
    }


    void AD9833::SetFrequency(Registers freqReg, double frequency)
    {
        if(frequency > 12.5e6) frequency = 12.5e6;
        if (frequency < 0) frequency = 0;

        if(freqReg == REG0) frequency0 = frequency;
        else frequency1 = frequency;

        uint32_t freqWord = (frequency * B28_Size)/(float)refFrequency;
        uint16_t upper14 = static_cast<uint16_t>((freqWord & 0xFFFC000) >> 14),
                 lower14 = static_cast<uint16_t>(freqWord & 0x3FFF);
        
        uint16_t reg = freqReg == REG0 ? FREQ0_WRITE_REG : FREQ1_WRITE_REG;
        lower14 |= reg;
        upper14 |= reg;

        WriteControlRegister();

        WriteRegister(lower14);
        WriteRegister(upper14);
    }

    void AD9833::IncrementPhase(Registers phaseReg, double phaseInDeg)
    {
        float phase = phaseReg == REG0 ? phase0 : phase1;
        SetPhase(phaseReg, phase + phaseInDeg);
    }

    void AD9833::SetPhase(Registers phaseReg, double phaseInDeg)
    {
        phaseInDeg = fmod(phaseInDeg,360);
        if(phaseInDeg < 0) phaseInDeg += 360;

        uint16_t phaseVal = static_cast<uint16_t>(BITS_PER_DEG * phaseInDeg) & 0x0FFF;
        phaseVal |= PHASE_WRITE_CMD;
        
        if(phaseReg ==REG0) phase0 = phaseInDeg;
        else {
            phase1 = phaseInDeg;
            phaseVal |= PHASE1_WRITE_REG;
        }
        WriteRegister(phaseVal);
        
    }

    void AD9833::SetWaveform(Registers reg, Waveform wavetype)
    {
        if (reg == REG0) waveForm0 = wavetype;
        else waveForm1 = wavetype;
        WriteControlRegister();
        
    }

    void AD9833::SetOutputSource(Registers freqReg, Registers phaseReg)
    {
        activeFreq = freqReg;
        if(phaseReg == Same_As_REG0) activePhase = activeFreq;
        else activePhase = phaseReg;
        WriteControlRegister();
        
    }

    void AD9833::EnableOutput(bool enable)
    {
        outputEnable = enable;
        WriteControlRegister();

    }

    void AD9833::SleepMode(bool enable)
    {
        DacDisabled = enable;
        IntClkDisabled = enable;
        WriteControlRegister();

    }

    void AD9833::DisableDAC(bool enable)
    {
        DacDisabled = enable;
        WriteControlRegister();

    }

    void AD9833::DisableInternalClock(bool enable)
    {
        IntClkDisabled = enable;
        WriteControlRegister();

    }

    double AD9833::GetActualProgrammedFrequency(Registers reg)
    {
        float frequency = reg == REG0 ? frequency0 : frequency1;
        int32_t freqWord = static_cast<uint32_t>(((frequency * B28_Size)/static_cast<double>(refFrequency)));
        return static_cast<double>(freqWord) * static_cast<double>(refFrequency) / (double)B28_Size;

    }

    double AD9833::GetActualProgrammedPhase(Registers reg)
    {
        double phase = reg == REG0 ? phase0 : phase1;
        uint16_t phaseVal = static_cast<uint16_t>(BITS_PER_DEG * phase) & 0x0FFF;
        cout << "Berechnete Hexp: " << std::hex << phaseVal << endl;
        return static_cast<double>(phaseVal)*(360.0 / 4096.0);
        
    }

    double AD9833::GetResolution(void)
    {
        return static_cast<double>(refFrequency) / static_cast<double>(B28_Size);

    }

//private
    void AD9833::WriteRegister(const uint16_t &data)
    {
        //zuerst 14LSB und dann 14MSB
        std::vector<uint8_t> puffer_send =
        {
            static_cast<uint8_t>(data>>8),
            static_cast<uint8_t>(data&0xFF)
            
        };

        DataTransmission(puffer_send);
    }

    void AD9833::WriteControlRegister(void)
    {
        uint16_t waveform;
        if(activeFreq == REG0)
        {
            waveform = waveForm0;
            waveform &= ~FREQ1_OUTPUT_REG;
        }
        else
        {
            waveform = waveForm1;
            waveform |= FREQ1_OUTPUT_REG;
        }

        if(activePhase == REG0) waveform &= ~PHASE1_OUTPUT_REG;
        else waveform |= PHASE1_OUTPUT_REG;

        if(outputEnable) waveform &= ~RESET_CMD;
        else waveform |= RESET_CMD;
        
        if(DacDisabled) waveform |= DISABLE_DAC;
        else waveform &= ~DISABLE_DAC;

        if(IntClkDisabled) waveform |= DISABLE_INT_CLOCK;
        else waveform &= ~DISABLE_INT_CLOCK;
        //sicherheitsmaßnahme dass Reservierte Bits == 0 gesetzt sind
        waveform &= ~RESERVED_BITS_0; 
        CRTLREG = waveform;
        
        WriteRegister(waveform);
    }
    

    void AD9833::DataTransmission(const std::vector<uint8_t> & txBuffer)
    {

        if(fileDescriptor < 0)
        {
            std::cerr << "Error: File Descripter wurde nicht geöffnet.";
            return ;
        }

        std::vector<uint8_t> rxBuffer = {0, 0};

        struct spi_ioc_transfer transfer ={};

        transfer.tx_buf = reinterpret_cast<unsigned long>(txBuffer.data());
        transfer.rx_buf = reinterpret_cast<unsigned long>(rxBuffer.data());
        transfer.len = txBuffer.size();
        transfer.speed_hz = sclkSpeed;
        transfer.bits_per_word = spiBitSize;
        transfer.delay_usecs = 1;

        if(ioctl(fileDescriptor, SPI_IOC_MESSAGE(1), &transfer)<0)
        {
            std::cerr << "ERROR: COM Fehler" <<std::endl;
            return;
        }
        if(0)
        {
            std::cout   << "Send: 0x" 
                        << std::hex << std::setw(2) << std::setfill('0') << (int)txBuffer[0]
                        << std::hex << std::setw(2) << std::setfill('0') << (int)txBuffer[1]
                        << endl;
        }

    }
