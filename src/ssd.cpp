#include "ssd.h"
// #include "SD.h"
#include "SPI.h"
#include "utils.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <FS.h>
#include <SD_MMC.h>
#include <cstdint>

#define EEPROM_SIZE 1
#define EEPROM_ADRRESS 0

#pragma pack(push, 1)
typedef struct Header
{
    uint16_t signature;
    int fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
} Header_t;
#pragma pack(pop)

typedef struct InfoHeader
{

    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t ImageSize;
    uint32_t XpixelsPerM;
    uint32_t YpixelsPerM;
    uint32_t colorsUsed;
    uint32_t ColorsImport;

} InfoHeader_t;

int pictureNumber = 0;

Ssd::Ssd()
{
}

void Ssd::initSD()
{

    if (!SD_MMC.begin("/sdcard", true))
    {
        Serial.println("SD Card Mount Failed");
        return;
    }
    activeSD = true;
}

void Ssd::disableSD()
{
    SD_MMC.end();
    activeSD = false;
}

void Ssd::saveImage(uint8_t *data, size_t dlength)
{

    if (!activeSD)
    {
        initSD();
    }

    EEPROM.begin(EEPROM_SIZE);

    pictureNumber = EEPROM.read(0) + 1;

    String path = "/picture" + String(pictureNumber) + ".bmp";

    fs::FS &fs = SD_MMC;

    LOG(LOG_TYPE_SD, path.c_str(), 0);
    File file = fs.open(path.c_str(), FILE_WRITE);

    if (!file)
    {
        LOG(LOG_TYPE_SD, "Failed to open file in writing mode", 0);
    }
    else
    {
        file.write(data, dlength); // payload (image), payload length
        LOG(LOG_TYPE_SD, "Saved file to path", 0);
        EEPROM.write(0, pictureNumber);
        EEPROM.commit();
    }

    file.close();

    LOG(LOG_TYPE_SD, "Saved image;", 0);

    disableSD();
}

void Ssd::LOG(const char *type_message, const char *message, bool close)
{

    if (!activeSD)
    {
        initSD();
    }

    String line = String(type_message) + String(message);
    File file = SD_MMC.open(LOG_FILE, FILE_APPEND);
    file.println(line);
    file.close();

    if (close)
    {
        disableSD();
    }
}

uint8_t ***Ssd::loadImage(int *height, int *width)
{

    if (!activeSD)
    {
        initSD();
    }

    EEPROM.begin(1);

    int number = EEPROM.read(0);

    String path = "/picture" + String(number) + ".bmp";

    fs::FS &fs = SD_MMC;

    File image = fs.open(path.c_str(), FILE_READ);

    LOG(LOG_TYPE_SD, path.c_str(), 0);

    if (!image)
    {
        LOG(LOG_TYPE_SD, "Failed to open file in reading mode", 0);
    }

    uint8_t imgInf[sizeof(InfoHeader_t)];
    uint8_t imgHea[sizeof(Header_t)];

    Header_t header;
    InfoHeader_t infoHeader;

    image.read(imgHea, sizeof(Header_t));
    memcpy(&header, imgHea, sizeof(Header_t));

    image.read(imgInf, sizeof(InfoHeader_t));
    memcpy(&infoHeader, imgInf, sizeof(InfoHeader_t));

    image.seek(header.dataOffset);

    *width = infoHeader.width;
    *height = -1 * infoHeader.height; //< 0 ? infoHeader.height * -1: infoHeader.height;

    uint8_t ***Pixels = (uint8_t ***)ps_malloc(*height * sizeof(uint8_t ***));

    for (int x = 0; x != *height; x++)
    {

        Pixels[x] = (uint8_t **)ps_malloc(*width * sizeof(uint8_t **));

        for (size_t y = 0; y != *width; y++)
        {

            Pixels[x][y] = (uint8_t *)ps_malloc(3 * sizeof(uint8_t));
        }
    }

    for (int x = 0; x < *height; x++)
    {
        for (int y = 0; y < *width; y++)
        {

            uint8_t R;
            uint8_t G;
            uint8_t B;

            image.read(&B, sizeof(B));
            image.read(&G, sizeof(G));
            image.read(&R, sizeof(R));

            Pixels[x][y][0] = R;
            Pixels[x][y][1] = G;
            Pixels[x][y][2] = B;
        }
    }

    image.close();

    LOG(LOG_TYPE_SD, "Image Load to PSRAM;", 0);

    disableSD();

    return Pixels;
}

void Ssd::imageFree(uint8_t ***Pixels, int height, int width)
{

    for (int x = 0; x != height; x++)
    {
        for (size_t y = 0; y != width; y++)
        {

            free(Pixels[x][y]);
        }
        free(Pixels[x]);
    }

    free(Pixels);

    Pixels = NULL;
}
