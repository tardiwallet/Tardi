#include "SimpleButtonsWindow.h"
#include "Button.h"

static const char *TAG = "SimpleButtonsWindow";

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "esp_system.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "lvgl_helpers.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

void click(void *arg)
{
    if (!arg)
        return;
    Button *btn = (Button *)arg;
    ESP_LOGI(TAG, "Key%d clicked", btn->get_id());
}

void SimpleButtonsWindow::show()
{
    ESP_LOGI(TAG, "show view.");
    // Get the current screen
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);
    // Create a Label on the currently active screen
    lv_obj_t *pLabel = lv_label_create(scr);
    // Modify the Label's text
    lv_label_set_text(pLabel, msg);
    // Align the Label to the center
    // NULL means align on parent (which is the screen now)
    // 0, 0 at the end means an x, y offset after alignment
    lv_obj_align(pLabel, NULL, LV_ALIGN_CENTER, 0, 0);

    bindKeys();

}

void SimpleButtonsWindow::createKeyEventsTask()
{
    xTaskCreatePinnedToCore(keyEventsTask, "keyEventsTask", 2048, this, 5, NULL, 1);
}

void SimpleButtonsWindow::keyEventsTask(void *arg)
{
    SimpleButtonsWindow *sbv = (SimpleButtonsWindow *)arg;
    sbv->activeTasks++;
    KeyActions ka;
    while (!sbv->closing)
    {
        if (xQueueReceive(sbv->keysQueue, &ka, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Key%d action received", ka.id);
            if (ka.id == 3 && ka.action == KEY_SHORT_PRESS)
            {

            }
            else if (ka.id == 1 && ka.action == KEY_SHORT_PRESS)
            {
                sbv->close();
            }
            else if (ka.action == KEY_LONG_PRESS)
            {
                ESP_LOGI(TAG, "Key%d long clicked", ka.id);
            }
        }
        else
        {
            // ESP_LOGI(TAG, "No Key received");
        }
    }
    sbv->activeTasks--;
    ESP_LOGI(TAG, "keyEventsTask exits.");
    vTaskDelete(NULL);
}
