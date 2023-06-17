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

class DisplayQRWindow : public Window, public KeyBinder
{
private:
    const char *caption = "";
    std::string qrData = "";
    std::vector<std::string> qrFragments;

    lv_obj_t *pLabelCaption = NULL;
    lv_obj_t *canvas = NULL;
    
    void createKeyEventsTask() override;
    static void keyEventsTask(void* arg);

public:
    DisplayQRWindow(NavButtons *nb, std::string qrData);
    void show() override;
    void close() override;
    void update();

    void setCaption(const char *caption) { caption = caption; }
};



