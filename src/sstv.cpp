#include "sstv.h"
#include "ssd.h"
#include <cstdint>

volatile long syncTime;
volatile float Y_even;
volatile float Y_odd;
volatile float RY;
volatile float BY;
volatile uint16_t syncPulse;
volatile uint16_t syncPorch;
volatile uint16_t sepaPulse;
volatile uint16_t porch;
volatile uint16_t RByScan;
volatile uint16_t YScan;
volatile uint16_t lineScan;

SSTv::SSTv(float freq)
{

    vspi = new SPIClass(HSPI);

    radio = new Module(NSS, RADIOLIB_NC, RST, RADIOLIB_NC, *vspi);

    audio = new AFSKClient(&radio, RADIOLIB_NC);

    AD = new MD_AD9833(DATA, CLK, FSYNC);

    phyLayer = audio->phyLayer;

    _freq = freq;
}

void SSTv::begin(float corretion, SStvMode mode)
{

    vspi->begin();

    AD->begin();
    AD->setFrequency(MD_AD9833::CHAN_0, 400);

    // initialize SX1278 with default settings
    Serial.print(F("[SX1278] Initializing ... "));
    int state = radio.beginFSK(_freq);
    // radio.setOOK(true);
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true)
        {
        };
    }

    this->configTimer(corretion, mode);

    phyLayer->startDirect();
}

void SSTv::close()
{

    vspi->end();
}

void SSTv::tone(float freq, unsigned long time)
{

    syncTime = micros();
    AD->setFrequency(MD_AD9833::CHAN_0, freq);
    while (micros() - syncTime < time)
    {
    }
}

void SSTv::visCode(VIS mode)
{

    phyLayer->transmitDirect();

    switch (mode)
    {

    case VISRB36:

        this->tone(1900, 300000); // Leader tone
        this->tone(1200, 10000);  // Break
        this->tone(1900, 300000); // Leader tone
        this->tone(1200, 30000);  // VIS Bit Start

        this->tone(1300, 90000); // 0 0 0
        this->tone(1100, 30000); // 1
        this->tone(1300, 90000); // 0 0 0

        this->tone(1100, 30000); // Even parity
        this->tone(1200, 30000); // VIS Stop
        audio->noTone();
        break;

    case VISSC180:

        this->tone(1900, 300000); // Leader tone
        this->tone(1200, 10000);  // Break
        this->tone(1900, 300000); // Leader tone

        this->tone(1200, 30000);
        this->tone(1100, 90000);
        this->tone(1300, 30000);
        this->tone(1100, 60000);
        this->tone(1300, 30000);

        this->tone(1100, 30000);
        this->tone(1200, 30000);
        audio->noTone();

    default:

        break;
    }
}

void SSTv::sendImage(uint8_t *Pixels, uint16_t height, uint16_t width, SStvMode mode)
{

    switch (mode)
    {

    case ROBOT36:
        robot36(Pixels, height, width);
        break;
    case SC180:
        sc180(Pixels, height, width);
        break;

    default:

        break;
    }
}

void SSTv::robot36(uint8_t *Pixels, uint16_t height, uint16_t width)
{

    uint16_t nLine = 0;
    uint16_t lineEven, lineOdd;

    visCode(VISRB36);

    phyLayer->transmitDirect();

    while (nLine != height)
    {

        lineEven = nLine;
        lineOdd = nLine + 1;

        this->tone(1200, syncPulse);
        this->tone(1500, syncPorch);

        for (size_t i = 0; i < width; i++)
        {
            int indx = (lineEven * width + i) * 3;
            Y_even = 16.0 +
                     (.003906 * ((65.738 * Pixels[indx]) + (129.057 * Pixels[indx + 1]) + (25.064 * Pixels[indx + 2])));
            this->tone(1500 + (Y_even * 3.1372), YScan);
        }

        this->tone(1500, sepaPulse);
        this->tone(1900, porch);

        for (size_t i = 0; i < width; i++)
        {

            int indx = (lineEven * width + i) * 3;
            RY = 128.0 +
                 (.003906 * ((112.439 * Pixels[indx]) + (-94.154 * Pixels[indx + 1]) + (-18.285 * Pixels[indx + 2])));
            this->tone(1500 + (RY * 3.1372), RByScan);
        }

        this->tone(1200, syncPulse);
        this->tone(1500, syncPorch);

        for (size_t i = 0; i < width; i++)
        {

            int indx = (lineOdd * width + i) * 3;
            Y_odd = 16.0 +
                    (.003906 * ((65.738 * Pixels[indx]) + (129.057 * Pixels[indx + 1]) + (25.064 * Pixels[indx + 2])));

            this->tone(1500 + (Y_odd * 3.1372), YScan);
        }

        this->tone(2300, sepaPulse);
        this->tone(1900, porch);

        for (size_t i = 0; i < width; i++)
        {

            int indx = (lineOdd * width + i) * 3;
            BY = 128.0 +
                 (.003906 * ((-37.945 * Pixels[indx]) + (-74.494 * Pixels[indx + 1]) + (112.439 * Pixels[indx + 2])));
            this->tone(1500 + (BY * 3.1372), RByScan);
        }

        nLine += 2;
    }

    audio->noTone();
    vspi->end();
}

void SSTv::sc180(uint8_t *Pixels, uint16_t height, uint16_t width)
{

    visCode(VISSC180);

    phyLayer->transmitDirect();

    for (size_t h = 0; h < height; h++)
    {

        this->tone(1200, syncPulse);
        this->tone(1500, porch);

        for (size_t i = 0; i < width; i++)
        {

            int indx = (h * width + i) * 3;
            this->tone(1500 + float(Pixels[indx] * 3.1372), lineScan);
        }

        for (size_t i = 0; i < width; i++)
        {
            int indx = (h * width + i) * 3;
            this->tone(1500 + float(Pixels[indx + 1] * 3.1372), lineScan);
        }

        for (size_t i = 0; i < width; i++)
        {
            int indx = (h * width + i) * 3;
            this->tone(1500 + float(Pixels[indx + 2] * 3.1372), lineScan);
        }
    }

    audio->noTone();
    vspi->end();
}

void SSTv::configTimer(float corretion, SStvMode mode)
{

    switch (mode)
    {

    case ROBOT36:

        syncPulse = 9000; // 70
        syncPorch = 3000;
        sepaPulse = 4500;
        porch = 1500;
        YScan = 270;
        RByScan = 134;
        break;

    case SC180:

        syncPulse = 5521 * corretion;
        syncPorch = 500 * corretion;
        lineScan = 734 * corretion;
        break;

    default:

        break;
    }
}
