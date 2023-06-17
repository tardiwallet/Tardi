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

class ButtonsWindow : public Window, public KeyBinder
{
private:
    const char *msg;
    lv_obj_t *pLabelMessage;
    lv_obj_t *pBtnsContainer;
    std::vector<std::string> buttons;
    std::vector<lv_obj_t*> pButtons;
    int selectedButtonIndex = -1;
    lv_label_align_t captionAlign = LV_LABEL_ALIGN_CENTER;

    void createKeyEventsTask() override;
    static void keyEventsTask(void* arg);

public:
    ButtonsWindow(NavButtons *nb);
    void setCaption(const char *caption) { msg = caption; }
    void alignCaption(lv_label_align_t align) { captionAlign = align; }
    void addButton(std::string caption);
    int getSelectedButtonIndex() {return selectedButtonIndex;}
    void show();
    void update();
    void close() override;
};



