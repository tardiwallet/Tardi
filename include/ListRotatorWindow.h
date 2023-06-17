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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

class ListRotatorWindow : public Window, public KeyBinder
{
private:
    const char *msg;
    std::vector<std::string> items;
    static const int N = 6;
    static const int lineHeight = 30;
    static const int leftPad = 30;
    int current_index = -1;
    int nVisibleItems = 3;
    lv_obj_t *pLabelMessage;
    lv_obj_t *pList;

    void createKeyEventsTask() override;
    static void keyEventsTask(void* arg);

public:
    ListRotatorWindow(NavButtons* nb);
    void setMessage(const char *message) { msg = message; }
    void addItem(std::string item);
    void setVisibleItemsCount(int n);
    void setCurrentIndex(int ind);
    int getCurrentIndex() { return current_index; }
    void show();
    void update();
    void close() override;
};