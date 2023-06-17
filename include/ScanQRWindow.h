#pragma once

#include "Window.h"
#include "Button.h"
#include "NavButtons.h"
#include "KeyBinder.h"
#include <vector>
#include <string>

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

class ScanQRWindow : public Window, public KeyBinder
{
private:
    const char *caption = "Keep your code \nin front of the camera";
    std::vector<std::string> qrCodes;
    lv_obj_t *pLabelCaption = NULL;
    lv_obj_t *pLabelProgress = NULL;
    lv_obj_t *pCanvas = NULL;
    QueueHandle_t decodeQueue = NULL;


    void createKeyEventsTask() override;
    static void keyEventsTask(void* arg);

    static void decodeTask(void *arg);
    static void updateTask(void *arg);


public:
    ScanQRWindow(NavButtons* nb);
    ~ScanQRWindow() {
        vQueueDelete(decodeQueue);
    }
    void show() override;
    void close() override;
    void update();

    void setCaption(const char *caption) { caption = caption; }
    std::vector<std::string> getQR() { return qrCodes; }
    
};