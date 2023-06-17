/**
 * @file Camera.h
 *
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include "esp_camera.h"
#ifdef __cplusplus
}
#endif

class Camera
{
private:
    /* data */
    bool initialized;
    camera_fb_t *fb = NULL;
    pixformat_t pixformat = PIXFORMAT_GRAYSCALE; // PIXFORMAT_RGB565 too slow, PIXFORMAT_GRAYSCALE slow, PIXFORMAT_JPEG fast
    framesize_t framesize = FRAMESIZE_VGA; //FRAMESIZE_240X240, FRAMESIZE_SVGA, FRAMESIZE_HD

public:
    Camera();
    int frameWidth() const;
    int frameHeight() const;
    void setFormat(pixformat_t pixformat)  { this->pixformat = pixformat; }
    void setFramesize(framesize_t framesize) { this->framesize = framesize; }
    bool init();
    bool isInitialized() { return initialized; }
    camera_fb_t *get_fb();
    void return_fb();
};
