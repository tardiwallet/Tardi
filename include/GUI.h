#pragma once

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/* Littlevgl specific */
#include "lvgl.h"
#include "lvgl_helpers.h"

class Window;

class GUI
{
private:
    /* data */
    bool initialized;
    Window *currentView;
    BaseType_t taskCore;

public:
    GUI(BaseType_t core);
    ~GUI();
    bool init();
    bool isInitialized() { return initialized; }
    void update();
    void showView(Window *view);
    void closeView();
    void waitToClose(Window *view);
};
void takeLVGLSemaphore();
void giveLVGLSemaphore();
