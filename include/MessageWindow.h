#pragma once

#include "Window.h"
#include "esp_log.h"


class MessageWindow : public Window
{
private:
    const char *msg;

public:
    MessageWindow() : Window("MessageWindow") {}
    void setMessage(const char *message) { msg = message; }

    void show()
    {
        ESP_LOGI(this->name, "show windows.");
        // Get the current screen
        lv_obj_t *scr = lv_disp_get_scr_act(NULL);
        // Create a Label on the currently active screen
        lv_obj_t *pLabel = lv_label_create(scr, NULL);
        // Modify the Label's text
        lv_label_set_text(pLabel, msg);
        // Align the Label to the center
        // NULL means align on parent (which is the screen now)
        // 0, 0 at the end means an x, y offset after alignment
        lv_obj_align(pLabel, NULL, LV_ALIGN_CENTER, 0, 0);
    }
};
