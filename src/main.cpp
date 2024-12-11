

#include "Arduino.h" //Requerido
#include "SPI.h"     // Requerido
#include "cam.h"
#include "esp_heap_caps.h"
#include "sstv.h"
#include "utils.h"

Cam cam;
Ssd sd;
SSTv sstv(434.00);

uint8_t *Pixels = nullptr;
// unt8_t ***Pixels = NULL;
int height, width;

void setup()
{

    // Serial.begin(115200);

    while (true)
    {
        cam.init();
        cam.takePicture();
        Pixels = sd.loadImage();
        sstv.begin(1, ROBOT36);
        sstv.sendImage(Pixels, HEIGHT, WIDTH, ROBOT36);
        heap_caps_free(Pixels);

        delay(5000);
    }
}

void loop()
{
}
