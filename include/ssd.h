#ifndef ssd_h
#define ssd_h

#include "FS.h"
#include "esp_camera.h"
#include <cstddef>
#include <cstdint>

class Ssd
{

  public:
    Ssd();
    void saveImage(uint8_t *data, size_t dlength);
    void getImage(uint16_t num, uint8_t ***Pixels);

  private:
};

#endif
