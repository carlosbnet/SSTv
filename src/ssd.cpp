#include "ssd.h"
// #include "SD.h"
#include "SPI.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <FS.h>
#include <SD_MMC.h>

#define EEPROM_SIZE 1

int pictureNumber = 0;

Ssd::Ssd()
{
}

void Ssd::saveImage(uint8_t *data, size_t dlength)
{

    // SPIClass *hspi = new SPIClass(HSPI);

    // hspi->begin(14, 21, 13, 33);

    if (!SD_MMC.begin("/sdcard", true))
    {
        Serial.println("SD Card Mount Failed");
        return;
    }

    if (SD_MMC.cardType() == CARD_NONE)
    {
        Serial.println("No SD Card attached");
        return;
    }

    Serial.println("successfully initialized");

    EEPROM.begin(EEPROM_SIZE);

    pictureNumber = EEPROM.read(0) + 1;

    String path = "/picture" + String(pictureNumber) + ".bmp";

    fs::FS &fs = SD_MMC;

    Serial.printf("Picture file name: %s\n", path.c_str());

    File file = fs.open(path.c_str(), FILE_WRITE);

    if (!file)
    {
        Serial.println("Failed to open file in writing mode");
    }
    else
    {
        file.write(data, dlength); // payload (image), payload length
        Serial.printf("Saved file to path: %s\n", path.c_str());
        EEPROM.write(0, pictureNumber);
        EEPROM.commit();
    }
    Serial.println("successfully initialized");

    file.close();
    SD_MMC.end();
}
