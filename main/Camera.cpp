#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
}
#endif

#include "Camera.h"

// camera pins
#define CAMERA_MODULE_NAME "CUSTOM"
#define CAMERA_PIN_PWDN CONFIG_CAMERA_PIN_PWDN
#define CAMERA_PIN_RESET CONFIG_CAMERA_PIN_RESET
#define CAMERA_PIN_XCLK CONFIG_CAMERA_PIN_XCLK
#define CAMERA_PIN_SIOD CONFIG_CAMERA_PIN_SIOD
#define CAMERA_PIN_SIOC CONFIG_CAMERA_PIN_SIOC

#define CAMERA_PIN_D7 CONFIG_CAMERA_PIN_Y9
#define CAMERA_PIN_D6 CONFIG_CAMERA_PIN_Y8
#define CAMERA_PIN_D5 CONFIG_CAMERA_PIN_Y7
#define CAMERA_PIN_D4 CONFIG_CAMERA_PIN_Y6
#define CAMERA_PIN_D3 CONFIG_CAMERA_PIN_Y5
#define CAMERA_PIN_D2 CONFIG_CAMERA_PIN_Y4
#define CAMERA_PIN_D1 CONFIG_CAMERA_PIN_Y3
#define CAMERA_PIN_D0 CONFIG_CAMERA_PIN_Y2
#define CAMERA_PIN_VSYNC CONFIG_CAMERA_PIN_VSYNC
#define CAMERA_PIN_HREF CONFIG_CAMERA_PIN_HREF
#define CAMERA_PIN_PCLK CONFIG_CAMERA_PIN_PCLK

#define XCLK_FREQ_HZ 20000000
#define CAMERA_FB_COUNT 2

static const char *TAG = "Camera";

Camera::Camera() : initialized(false)
{
}


int Camera::frameWidth() const
{
    switch (framesize)
    {
    case FRAMESIZE_96X96:    // 96x96
        return 96;
    case FRAMESIZE_QQVGA:    // 160x120
        return 160;
    case FRAMESIZE_QCIF:     // 176x144
        return 176;
    case FRAMESIZE_HQVGA:    // 240x176
        return 240;
    case FRAMESIZE_240X240:  // 240x240
        return 240;
    case FRAMESIZE_QVGA:     // 320x240
        return 320;
    case FRAMESIZE_CIF:      // 400x296
        return 400;
    case FRAMESIZE_HVGA:     // 480x320
        return 480;
    case FRAMESIZE_VGA:      // 640x480
        return 640;
    case FRAMESIZE_SVGA:     // 800x600
        return 800;
    case FRAMESIZE_XGA:      // 1024x768
        return 1024;
    case FRAMESIZE_HD:       // 1280x720
        return 1280;
    case FRAMESIZE_SXGA:     // 1280x1024
        return 1280;
    case FRAMESIZE_UXGA:     // 1600x1200
        return 1600;
    default:
        return 0;
    }
    return 0;    
}

int Camera::frameHeight() const
{
    switch (framesize)
    {
    case FRAMESIZE_96X96:    // 96x96
        return 96;
    case FRAMESIZE_QQVGA:    // 160x120
        return 120;
    case FRAMESIZE_QCIF:     // 176x144
        return 144;
    case FRAMESIZE_HQVGA:    // 240x176
        return 176;
    case FRAMESIZE_240X240:  // 240x240
        return 240;
    case FRAMESIZE_QVGA:     // 320x240
        return 240;
    case FRAMESIZE_CIF:      // 400x296
        return 296;
    case FRAMESIZE_HVGA:     // 480x320
        return 320;
    case FRAMESIZE_VGA:      // 640x480
        return 480;
    case FRAMESIZE_SVGA:     // 800x600
        return 600;
    case FRAMESIZE_XGA:      // 1024x768
        return 768;
    case FRAMESIZE_HD:       // 1280x720
        return 720;
    case FRAMESIZE_SXGA:     // 1280x1024
        return 1024;
    case FRAMESIZE_UXGA:     // 1600x1200
        return 1200;
    default:
        return 0;
    }
    return 0;    
}

bool Camera::init()
{
    if (initialized)
        return initialized;

    ESP_LOGI(TAG, "Camera module initialization");
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAMERA_PIN_D0;
    config.pin_d1 = CAMERA_PIN_D1;
    config.pin_d2 = CAMERA_PIN_D2;
    config.pin_d3 = CAMERA_PIN_D3;
    config.pin_d4 = CAMERA_PIN_D4;
    config.pin_d5 = CAMERA_PIN_D5;
    config.pin_d6 = CAMERA_PIN_D6;
    config.pin_d7 = CAMERA_PIN_D7;
    config.pin_xclk = CAMERA_PIN_XCLK;
    config.pin_pclk = CAMERA_PIN_PCLK;
    config.pin_vsync = CAMERA_PIN_VSYNC;
    config.pin_href = CAMERA_PIN_HREF;
    config.pin_sccb_sda = CAMERA_PIN_SIOD;
    config.pin_sccb_scl = CAMERA_PIN_SIOC;
    config.pin_pwdn = CAMERA_PIN_PWDN;
    config.pin_reset = CAMERA_PIN_RESET;
    config.xclk_freq_hz = XCLK_FREQ_HZ;
    config.pixel_format = pixformat;
    config.frame_size = framesize;
    config.jpeg_quality = 12;
    config.fb_count = CAMERA_FB_COUNT;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return false;
    }
    sensor_t *s = esp_camera_sensor_get();
    if (s->id.PID == OV2640_PID)
    {
        // s->set_vflip(s, 1); // flip it back
        //     // s->set_hmirror(s, 0);
        //     // s->set_brightness(s, 2);
        //     // s->set_contrast(s, 3);
    }

    initialized = true;
    return initialized;
}

camera_fb_t *Camera::get_fb()
{
    if (!initialized)
        return NULL;
    // do not return a second frame buffer
    if (fb)
    {
        ESP_LOGE(TAG, "camera Framebuffer already in use.");
        return NULL;
    }
    fb = esp_camera_fb_get();
    if (fb == NULL)
    {
        ESP_LOGE(TAG, "camera Framebuffer is NULL.");
    }
    return fb;
}

void Camera::return_fb()
{
    if (!initialized)
        return;
    if (fb)
    {
        esp_camera_fb_return(fb);
        fb = NULL;
    }
}
