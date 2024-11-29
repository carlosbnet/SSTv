#include "img.h"
#include "EEPROM.h"
#include "FS.h"
// #include "SD.h"
#include "esp32-hal-psram.h"
#include <SD_MMC.h>

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

img::img()
{
}

uint8_t ***img::loadImage(int *height, int *width)
{

    // SPIClass *hspi = new SPIClass(HSPI);

    // hspi->begin(14, 21, 13, 33);

    if (!SD_MMC.begin("/sdcard", true))
    {
        Serial.println("Card Mount Failed");
        while (true)
        {
            Serial.println(".");
        }
    }

    EEPROM.begin(1);

    int number = EEPROM.read(0);

    String path = "/picture" + String(number) + ".bmp";

    fs::FS &fs = SD_MMC;

    File image = fs.open(path.c_str(), FILE_READ);

    if (!image)
    {
        Serial.println("Failed to open file in reading mode");
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

    SD_MMC.end();
    // hspi->end();

    Serial.println("Image Load to PSRAM.");
    return Pixels;
}

void img::imageFree(uint8_t ***Pixels, int height, int width)
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
