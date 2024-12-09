#include "cam.h"
#include "SPI.h"
#include "esp32-hal-psram.h"
#include "img_converters.h"
#include "sensor.h"
#include "utils.h"
#include <esp_camera.h>

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

Ssd sdC;

camera_config_t config;

Cam::Cam()
{
}

void Cam::init()
{

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;

    /*
      if (psramFound())
      {
          config.frame_size = FRAMESIZE_QVGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
          config.jpeg_quality = 10;
          config.fb_count = 2;
      }
      else
      {
          config.frame_size = FRAMESIZE_QVGA;
          config.jpeg_quality = 12;
          config.fb_count = 1;
      }

      // Init Camera
      esp_err_t err = esp_camera_init(&config);
      if (err != ESP_OK)
      {
          Serial.printf("Camera init failed with error 0x%x", err);
          return;
      }*/
}

void Cam::takePicture()
{

    // Init Camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        sdC.LOG(LOG_TYPE_CAM, "Camera init failed with error ", 1);
        return;
    }

    sdC.LOG(LOG_TYPE_CAM, "Camera init", 1);

    camera_fb_t *fb = NULL;

    fb = esp_camera_fb_get();

    if (!fb)
    {
        sdC.LOG(LOG_TYPE_CAM, "Camera capture failed", 1);
        return;
    }

    int length = 320 * 240 * 3;

    uint8_t *bufferBmp = (uint8_t *)ps_malloc(length * sizeof(uint8_t));
    size_t bufferLength = 0;

    frame2bmp(fb, &bufferBmp, &bufferLength);

    sdC.saveImage(bufferBmp, bufferLength);

    esp_camera_fb_return(fb);
    free(bufferBmp);

    esp_camera_deinit();

    sdC.LOG(LOG_TYPE_CAM, "Camera deinit", 1);
}
