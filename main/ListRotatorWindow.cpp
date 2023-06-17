#include "ListRotatorWindow.h"
#include "Button.h"
#include "GUI.h"

static const char *TAG = "ListRotatorWindow";

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

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

static lv_style_t style_list_default;
static lv_style_t style_btn_focused;
// static lv_style_t style_default;

ListRotatorWindow::ListRotatorWindow(NavButtons *nb) : Window("ListRotatorWindow"), KeyBinder(nb)
{
    takeLVGLSemaphore();
    lv_style_init(&style_list_default);
    lv_style_set_radius(&style_list_default, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_inner(&style_list_default, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&style_list_default, LV_STATE_DEFAULT, 0);
    // lv_style_init(&style_default);
    // lv_style_set_bg_color(&style_default, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    // lv_style_set_text_color(&style_default, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_style_init(&style_btn_focused);
    lv_style_set_bg_color(&style_btn_focused, LV_STATE_FOCUSED, LV_COLOR_YELLOW);
    lv_style_set_text_color(&style_btn_focused, LV_STATE_FOCUSED, LV_COLOR_BLUE);
    giveLVGLSemaphore();

    pLabelMessage = NULL;
    pList = NULL;
}

void ListRotatorWindow::createKeyEventsTask()
{
    xTaskCreatePinnedToCore(keyEventsTask, "keyEventsTask", 2048, this, 5, NULL, 1);
}

void ListRotatorWindow::show()
{
    ESP_LOGI(TAG, "show view.");

    // take and give semaphone not needed because show() is already within semaphore lock.
    // takeLVGLSemaphore();
    update();
    // giveLVGLSemaphore();

    bindKeys();
}

void ListRotatorWindow::setCurrentIndex(int ind)
{
    if (items.size() == 0)
        current_index = -1;
    else
    {
        while (ind < 0)
            ind += items.size();
        current_index = ind % items.size();
    }
    takeLVGLSemaphore();
    update();
    giveLVGLSemaphore();
}

void ListRotatorWindow::addItem(std::string item)
{
    items.push_back(item);

    if (items.size() == 0)
    {
        current_index = -1;
    }
    else
    {
        current_index = 0;
    }
}

void ListRotatorWindow::setVisibleItemsCount(int n)
{
    if (n <= N && n > 0)
    {
        nVisibleItems = n;
    }
    else
    {
        nVisibleItems = N;
    }
}

void ListRotatorWindow::close()
{
    ESP_LOGI(TAG, "closing view.");
    Window::close();
    KeyBinder::unbindKeys();
}

/**
 * @brief Method update() has to be called within takeLVGLSemaphore() and giveLVGLSemaphore().
 *
 */
void ListRotatorWindow::update()
{
    // Get the current screen
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);
    // lv_obj_set_style_bg_color(scr, lv_color_hex(0x003a57), LV_PART_MAIN);
    // lv_obj_set_style_text_color(scr, lv_color_hex(0xffffff), LV_PART_MAIN);


    // Create a Label on the currently active screen (only once)
    if (!pLabelMessage)
    {
        pLabelMessage = lv_label_create(scr, NULL);
        // Modify the Label's text
        lv_label_set_text(pLabelMessage, msg);
        // Align the Label to the center
        // NULL means align on parent (which is the screen now)
        // 0, 0 at the end means an x, y offset after alignment
        lv_obj_align(pLabelMessage, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    }


    // Initialize List (only once)
    if (!pList)
    {
        pList = lv_list_create(scr, NULL);
        lv_obj_add_style(pList, LV_OBJ_PART_MAIN, &style_list_default);
        lv_obj_set_size(pList, 230, 160);
        lv_obj_align(pList, NULL, LV_ALIGN_CENTER, 0, 0);
        // lv_obj_align(pList, LV_ALIGN_IN_LEFT_MID, leftPad, 30);
        // lv_list_set_edge_flash(pList, true);
        lv_list_set_scrollbar_mode(pList, LV_SCROLLBAR_MODE_OFF); //LV_SCROLLBAR_MODE_AUTO
        lv_list_set_layout(pList, LV_LAYOUT_COLUMN_LEFT);
        // LV_LIST_PART_SCROLLBAR

    }

    // Update the list's items
    if (lv_list_get_size(pList) != items.size())
    {
        lv_list_clean(pList);
        for (int i = 0; i < items.size(); i++) 
        {
            lv_obj_t * list_btn;
            list_btn = lv_list_add_btn(pList, NULL, items[i].c_str());
            lv_obj_add_style(list_btn, LV_BTN_PART_MAIN, &style_btn_focused);
            // lv_obj_set_event_cb(list_btn, event_handler);
        }
    }

    lv_obj_t* selectedBtn = lv_list_get_btn_selected(pList);
    int selectedIndex = !selectedBtn? -1: lv_list_get_btn_index(pList, selectedBtn);
    if (selectedIndex != current_index) 
    {
        
        lv_obj_t* nextBtn = NULL;
        for (int i = 0; i <= current_index; i++)
            nextBtn = lv_list_get_next_btn(pList, nextBtn);

        lv_list_focus(nextBtn, LV_ANIM_ON);
        lv_list_focus_btn(pList, nextBtn);
    }

}


void ListRotatorWindow::keyEventsTask(void *arg)
{
    ListRotatorWindow *lrv = (ListRotatorWindow *)arg;
    lrv->activeTasks++;
    KeyActions ka;
    while (!lrv->closing)
    {
        if (xQueueReceive(lrv->keysQueue, &ka, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Key%d action received", ka.id);
            if (ka.id == 3 && ka.action == KEY_SHORT_PRESS)
            {
                // scroll down
                int ind = lrv->getCurrentIndex();
                lrv->setCurrentIndex(ind + 1);
            }
            else if (ka.id == 1 && ka.action == KEY_SHORT_PRESS)
            {
                lrv->close();
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
    lrv->activeTasks--;
    ESP_LOGI(TAG, "lrv_process_keys exits.");
    vTaskDelete(NULL);
}
