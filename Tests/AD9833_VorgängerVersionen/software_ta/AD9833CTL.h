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
#include <bitset>


using namespace std;

class CTLREG
{
private:
    uint16_t word;

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

    CTLREG();
    //~CTLREG();

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
};

class WAVECONFIG
{
private:

    static constexpr uint32_t MCLK = 25000000;

    static constexpr double TWO_PI = 2 * M_PI;
    static constexpr double EPSILON = 1e-6;

    static constexpr uint32_t MSB{0xFFFF0000};

    static constexpr uint32_t LSB{0x0000FFFF};

    //calculate 28Bit frequency word
    //later to private
    //Return: uint32_t
    //Attribute: uint64_t frequency
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
    //later to private
    //Return: uint16_t
    //Attribute: double phase
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
    CTLREG& clt;
    WAVECONFIG(CTLREG& ctlRef);

    //print calculated frequency word
    //Return: VOid
    //Attribute: uint64_t frequency
    void printFreqStream(uint64_t frequency);

    //print calculated phase word
    //Return: void
    //Attribute: double phase
    void printPhaseStream(double phase);

    //print configured frequency word ready for SPI transmission
    //Return: void
    //Attribute: uint32_t freq, bool freqReg
    void printConfFWord(uint32_t freq, bool freqReg);

    //configure frequency word ready for SPI transmission
    //Return: uint32_t
    //Attribute: uint32_t freq, bool freqReg
    uint32_t confFWord(uint32_t freq, bool freqReg);

};



/*
class AD9833
{
private:
    //SPI Konstanten
    std::string DEVICE = "";
    int SPI_FD;
    const uint32_t SPI_SPEED = 1000;  
    uint8_t SPI_MODE = SPI_MODE_1;
    uint8_t FLAGE = O_RDWR;


    void CREATE_FREQ_WORD(uint64_t &frequenz)
    {
        assert((frequenz < 12500000) && "ERROR: max Output Freq 12,5MHz\n");
        static_cast<uint32_t>((frequenz*((1<<28)-1))/MCLK);
    }

    //Create whole word.
    uint32_t CREATE_FREQ_WORD(uint64_t frequenz)
    {
        assert((frequenz < 12500000) && "ERROR: max Output Freq 12,5MHz\n");
        return static_cast<uint32_t>((frequenz*((1<<28)-1))/MCLK);
    }
    uint16_t CREATE_PHASE_WORD(double phase = 0)
    {
        uint16_t word{0};
        while(phase < 0) phase += TWO_PI;
        if (std::fabs(std::fmod(phase, TWO_PI)) < EPSILON) word = static_cast<uint16_t>(phase/(2.0*M_PI)*4096.0);
        return word;
    }


    // build freq and phase word
    void setupBitStream32(uint16_t setRegBit, uint32_t frequence = 100, uint16_t phase = 0)
    {
        uint32_t freq_bits = CREATE_FREQ_WORD(freq_bits);
        //check if the frequence is 14 Bit size or 28 Bit size and setup ctl reg
        if((freq_bits & MSB_14) != 0 && (freq_bits & LSB_14) != 0)
        {
            sendSPIData(B28);
            std::cout << "CTRL REG: Send B28 Bit\n";
        }

        //return variable include bitstream
        uint32_t word = 0x00000000;

        //setup phase % MOD 2pi
        while(phase < 0) phase += TWO_PI;
        if (std::fabs(std::fmod(phase, TWO_PI)) < EPSILON)
        {
            word = static_cast<uint16_t>(phase/(2.0*M_PI)*4096.0);
            if((setRegBit&PHASE0)==PHASE0) word |= PHASE0;
            if((setRegBit&PHASE1)==PHASE1) word |= PHASE1;
            sendSPIData(word);
        }

        //check Freq Reg 0 1
        if ((setRegBit & FREQ1) == FREQ1 || (setRegBit & FREQ0) == FREQ0)
        {
            assert((frequence < 12500000) && "ERROR: max Output Freq 12,5MHz\n");
            if((freq_bits & MSB_14) == freq_bits)
            {
                sendSPIData(HLB);
                std::cout << "CTRL REG: Send HLB Bit MSB\n";
            }
            if((freq_bits & LSB_14) == freq_bits)
            {
                sendSPIData(0x0000);
                std::cout << "CTRL REG: Send HLB Bit LSB\n";
            }
            word = setRegBit | freq_bits;
            sendSPIData(word);
        }

        //check CTL Reg
        if ((setRegBit & CHANGE_REG) == CHANGE_REG)
        {
            std::cerr << "ERROR: SetupBit equal to CTRL REG " << std::endl;
            return;
        }
        return;
    }

    
    void sendSPIData(const uint32_t& data)
    {
        uint32_t verify_data_typ = data; 
        uint8_t tx_len = 1;
        std::array<uint16_t,2> tx_buffer = {0x0000, 0x0000};
        std::array<uint16_t,2> rx_buffer = {0x0000, 0x0000};

        if (SPI_FD < 0)
        {
            std::cerr << "ERROR: SPI weren't opened" << std::endl;
            return;
        }

        //checking for 16Bit word
        if ((verify_data_typ & 0xFFFF) == verify_data_typ)
        {
            tx_buffer[0] = static_cast<uint16_t>(data);
        }
        //checking for 32Bit word
        else 
        {
            tx_buffer[0] = static_cast<uint16_t>((data >> 16) & 0xFFFF);
            tx_buffer[1] = static_cast<uint16_t>(data & 0xFFFF);
            tx_len = 2;
        }
        
        struct spi_ioc_transfer spi_transfer = {};

        spi_transfer.tx_buf = reinterpret_cast<unsigned long>(tx_buffer.data());
        spi_transfer.rx_buf = reinterpret_cast<unsigned long>(rx_buffer.data());
        spi_transfer.len = tx_len * sizeof(uint16_t);
        spi_transfer.speed_hz = SPI_SPEED;
        spi_transfer.bits_per_word = 8;
        spi_transfer.delay_usecs = 0;

        if (ioctl(SPI_FD, SPI_IOC_MESSAGE(1), &spi_transfer) < 0)
        {
            std::cerr << "ERROR: Couldn't send SPI-Data" << std::endl;
            return;
        }
        std::cout << "Send: 0x" << std::hex << (int)tx_buffer[0]
                << " | Receive: 0x" << (int)rx_buffer[0] << std::endl;
    }



    
public:
    void setAD9833(uint32_t frequence, uint16_t phase);


    //uint16_t b28 = B28;
    //Initailisieurng der SPI Kommunikation
    AD9833();
    //Dekonstruktor
    ~AD9833();

    void SPI_SETUP(const std::string& devicePath, const uint8_t& spi_mode, const uint8_t& flag);

    //First Aktion of AD9833
    void initAD9833();

    //Set Frequency0
    void setFrequency0(uint32_t frequency, uint32_t phase);

    //Set Frequency1
    void setFrequency1(uint32_t frequency, uint32_t phase);

    // Prüft, ob das Gerät erfolgreich geöffnet wurde
    bool isOpen() const;
};*/

#endif