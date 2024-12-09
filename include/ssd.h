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
    uint8_t ***loadImage(int *height, int *width);
    void initSD();
    void disableSD();
    bool activeSD = false;
    void LOG(const char *type_message, const char *message);
    void imageFree(uint8_t ***Pixels, int height, int width);

  private:
};

#endif
