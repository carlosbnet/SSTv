#ifndef img_h
#define img_h

#include "SPI.h"

class img
{
  public:
    img();
    uint8_t ***loadImage(int *height, int *width);
    void imageFree(uint8_t ***Pixels, int height, int width);

  private:
};

#endif
