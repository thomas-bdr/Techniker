#ifndef AD9833_LIB_H
#define AD9833_LIB_H

#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <array>

enum class ControlFlags : uint16_t {
    B28     = 1 << 13,
    HLB     = 1 << 12,
    FSELECT = 1 << 11,
    PSELECT = 1 << 10,
    RESET   = 1 << 8,
    SLEEP1  = 1 << 7,
    SLEEP12 = 1 << 6,
    OPBITEN = 1 << 5,
    DIV2    = 1 << 3,
    MODE    = 1 << 1,
};

constexpr ControlFlags operator|(ControlFlags a, ControlFlags b) 
{
    return static_cast<ControlFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}


class AD9833
{
private:
    //AD9833 Konstanten CTRL_REG
    //static constexpr uint16_t SET_CTRL_REG{0x2000};
    static constexpr uint16_t CHANGE_REG{3<<14};
    static constexpr uint16_t B28{1<<13};
    static constexpr uint16_t HLB{1<<12};

    static constexpr uint16_t FSELECT{1<<11};
    static constexpr uint16_t PSELECT{1<<10};

    static constexpr uint16_t RESET{1<<8};

    static constexpr uint16_t SLEEP1{1<<7};
    static constexpr uint16_t SLEEP12{1<<6};

    static constexpr uint16_t OPBITEN{1<<5};
    static constexpr uint16_t DIV2{1<<3};
    static constexpr uint16_t MODE{1<<1};

    //Bitmask FREQ_REG
    static constexpr uint16_t FREQ0{1<<14};
    static constexpr uint16_t FREQ1{1<<15};

    //Bitmask Phase_REG
    static constexpr uint16_t PHASE0{3<<14};
    static constexpr uint16_t PHASE1{7<<13};

    static constexpr uint32_t MCLK = 25000000;

    static constexpr double TWO_PI = 2 * M_PI;
    static constexpr double EPSILON = 1e-6;

    static constexpr uint32_t MSB_14{0xFFFF0000};

    static constexpr uint32_t LSB_14{0x0000FFFF};


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

    uint32_t bitStream(uint16_t CTL_WORD, uint32_t word)
    {

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
    
    //ctl Reg
    uint16_t ctlReg{0};
    void setFlag(ControlFlags flag);
    void clearFlag(ControlFlags flag);
    uint16_t getControlWord() const;
    void sendControllRegister(uint16_t controlWord);
    //setup  CTL_REG
    //Reset, B28, HLB, FSELECT, PSELECT, SLEEP1, SLEEP12, OPBITEN, DIV2, MODE
    void configureAD9833(bool reset=0, bool b28=0, bool hlb=0, 
        bool fselect=0,bool pselect=0, bool sleep1=0, bool sleep12=0, 
        bool opbiten=0, bool div2=0, bool mode=0);

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
};

#endif