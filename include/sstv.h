#ifndef sstv_h
#define sstv_h

#include <MD_AD9833.h>
#include <RadioLibC.h>

#define NSS 15
#define RST 4

#define DATA 3
#define CLK 1
#define FSYNC 2

enum VIS
{

    VISRB36 = 0,
    VISSC180 = 1,
};

enum SStvMode
{

    ROBOT36 = 0,
    SC180 = 1,
};

class SSTv
{

  public:
    SSTv(float freq);
    void begin(float corretion, SStvMode mode);

    void tone(float freq, unsigned long time);
    void visCode(VIS mode);
    void sendImage(uint8_t *Pixels, uint16_t height, uint16_t width, SStvMode mode);
    void robot36(uint8_t *Pixels, uint16_t height, uint16_t width);
    void sc180(uint8_t *Pixels, uint16_t height, uint16_t width);
    void configTimer(float corretion, SStvMode mode);
    void close();
    SPIClass *vspi;
    PhysicalLayer *phyLayer;
    SX1278 radio = NULL;
    float _freq;
    MD_AD9833 *AD;
    AFSKClient *audio;
};

#endif
