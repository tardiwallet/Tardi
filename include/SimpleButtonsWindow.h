#pragma once

#include "Window.h"
#include "Button.h"
#include "NavButtons.h"
#include "KeyBinder.h"

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

class SimpleButtonsWindow : public Window, public KeyBinder
{
private:
    const char *msg;
    lv_indev_t *indev_button;

    static void keyEventsTask(void* arg);
    void createKeyEventsTask() override;

public:
    SimpleButtonsWindow(NavButtons *nb) : Window("SimpleButtonsWindow"), KeyBinder(nb)
    {
    }

    void setMessage(const char *message) { msg = message; }
    void show() override;
    void close() override {
        Window::close();
        KeyBinder::unbindKeys();
    }
};