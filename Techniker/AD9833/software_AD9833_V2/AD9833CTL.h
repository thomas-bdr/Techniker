#ifndef AD9833CTL_H
#define AD9833CTL_H

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

class AD9833
{
public:

    AD9833();
    ~AD9833();

// ///////////////////////// //
// /////Control Reg Func//// //
// ///////////////////////// //

private:
    volatile uint16_t word = 0;

public:
    enum Flags : uint16_t
    {
        B28 = 1<<13,
        HLB = 1<<12,
        FSELECT = 1<<11,
        PSELECT = 1<<10,
        RESET = 1<<8,
        SLEEP1 = 1<<7,
        SLEEP12 = 1<<6,
        OPBITEN = 1<<5,
        DIV2 = 1<<3,
        MODE = 1<<1,

        PHASE0 = 3<<14,
        PHASE1 = 7<<13,

        FREQ0 = 1<<14,
        FREQ1 = 1<<15,

    };

    //Set an Flag of ctl word
    //Return: void
    //Attribute: Flags flag
    void setFlag(Flags flag);

    //clear Flag of ctl word
    //Return: void
    //Attribute: Flags flag
    void clearFlag(Flags flag);

    //check state of flag
    //Return: bool
    //Attribute: Flags flag
    bool isFlagSet(Flags flag);

    //output the current ctl word
    //Return: uint16_t
    //Attribute: Flags flag
    uint16_t getword() const;

    //print out the current ctl word
    //Return: void
    //Attribute: Flags flag
    void print() const;

// ///////////////////////// //
// /////Waveform Generat//// //
// ///////////////////////// //
private:
    static constexpr uint32_t MCLK = 25000000;

    static constexpr double TWO_PI = 2 * M_PI;
    static constexpr double EPSILON = 1e-6;

    static constexpr uint32_t MSB{0xFFFF0000};

    static constexpr uint32_t LSB{0x0000FFFF};

    //calculate 28Bit frequency word
    uint32_t calcFreq(uint64_t frequency)
    {
        if(frequency > 12500000)
        {
            std::cerr << "ERROR: Output Frequency Overload. Max. 12.5MHz";
        return -1;
        }
        return (frequency*((1<<28)-1))/MCLK;
    }

    //calculate 16Bit phase word
    uint16_t calcPhase(double phase = 0)
    {
        uint16_t pword{0};
        phase =std::fabs(std::fmod(phase, TWO_PI));
    /*
        * if (phase < EPSILON || phase - TWO_PI < EPSILON)
        * phase = 0;
        */
        phase = std::round(phase*1000)/1000;
        pword = static_cast<uint16_t>(phase/(2.0*M_PI)*4096.0);
        return pword;
    }   

public:
    //print calculated frequency word
    //Return: VOid
    //Attribute: uint64_t frequency
    void printFreqStream(uint64_t frequency);

    //print calculated phase word
    //Return: void
    //Attribute: double phase
    void printPhaseStream(double phase);

    //print configured phase word
    //Return: void
    //Attribute: double phase, bool phaseReg
    void printConfPhaseWord(double phase, Flags phaseReg);

    //configure phase word
    //Return: void
    //Attribute: double phase, bool phaseReg
    uint16_t confPhaseWord(double phase, Flags type);

    //print configured frequency word ready for SPI transmission
    //Return: void
    //Attribute: uint32_t freq, bool freqReg
    void printConfFWord(uint32_t freq, Flags type);

    //configure frequency word ready for SPI transmission
    //Return: uint32_t
    //Attribute: uint32_t freq, bool freqReg
    uint32_t confFWord(uint32_t freq, Flags type);

// ///////////////////////// //
// /////SPI lib function//// //
// ///////////////////////// // 

private:

    const char *device = "/dev/spidev0.0";
    int spi_fd = -1;
    uint8_t mode = SPI_MODE_2;
    uint8_t bits = 8;         // 8 oder 16 Bit pro Wort
    uint32_t speed = 50000;   // SPI Geschwindigkeit in Hz

    void SPI_sendData(const std::vector<uint8_t> &tx_buffer, std::vector<uint8_t> &rx_buffer)
    {

        if (spi_fd < 0)
        {
            std::cerr << "ERROR: SPI weren't opened" << std::endl;
            return;
        }
        rx_buffer.assign(tx_buffer.size(),0);
        struct spi_ioc_transfer spi_transfer = {};

        spi_transfer.tx_buf = reinterpret_cast<unsigned long>(tx_buffer.data());
        spi_transfer.rx_buf = reinterpret_cast<unsigned long>(rx_buffer.data());
        spi_transfer.len = tx_buffer.size();
        spi_transfer.speed_hz = speed;
        spi_transfer.bits_per_word = bits;
        spi_transfer.delay_usecs = 50;

        if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 0)
        {
            std::cerr << "ERROR: SPI-Com failed." << std::endl;
            return;
        }
        if(0)
        {
        std::cout << "Send: 0x" << std::hex << (int)tx_buffer[0]
                << " | Receive: 0x" << (int)rx_buffer[0] << std::endl;
        }
    }
    
public:

    //Transfer 16Bit word to IC
    //Return: void
    //Attribute: data
    void SPI_sendData16Bit(const uint16_t& data);

    //Transfer 32Bit word to IC
    //Return: void
    //Attribute: data
    void SPI_sendData32Bit(const uint32_t& data);


// ///////////////////////// //
// /////Sequenc function//// //
// ///////////////////////// //  

};

#endif