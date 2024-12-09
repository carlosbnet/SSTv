

#include "Arduino.h" //Requerido
#include "SPI.h"     // Requerido
#include "cam.h"
#include "sstv.h"

Cam cam;
Ssd sd;
SSTv sstv(434.00);

uint8_t ***Pixels = NULL;
int height, width;

void setup()
{

    // Serial.begin(115200);

    while (true)
    {
        cam.init();
        cam.takePicture();
        Pixels = img.loadImage(&height, &width);
        sstv.begin(1, ROBOT36);
        sstv.sendImage(Pixels, height, width, ROBOT36);
        img.imageFree(Pixels, height, width);

        delay(5000);
    }
}

void loop()
{
}
