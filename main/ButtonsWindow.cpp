#include "ButtonsWindow.h"
#include "Button.h"
#include "GUI.h"

static const char *TAG = "ButtonsWindow";

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

static lv_style_t style_btn_pressed;

ButtonsWindow::ButtonsWindow(NavButtons *nb) : Window("ButtonsWindow"), KeyBinder(nb)
{
    takeLVGLSemaphore();
    lv_style_init(&style_btn_pressed);
    lv_style_set_bg_color(&style_btn_pressed, LV_STATE_PRESSED, LV_COLOR_YELLOW);
    lv_style_set_text_color(&style_btn_pressed, LV_STATE_PRESSED, LV_COLOR_BLUE);
    giveLVGLSemaphore();

    pLabelMessage = NULL;
}

void ButtonsWindow::createKeyEventsTask()
{
    xTaskCreatePinnedToCore(keyEventsTask, "keyEventsTask", 2048, this, 5, NULL, 1);
}

void ButtonsWindow::close()
{
    ESP_LOGI(TAG, "closing view.");
    Window::close();
    KeyBinder::unbindKeys();
}

void click(void *arg)
{
    if (!arg)
        return;
    Button *btn = (Button *)arg;
    ESP_LOGI(TAG, "Key%d clicked", btn->get_id());
}

void ButtonsWindow::show()
{
    ESP_LOGI(TAG, "show view.");

    // take and give semaphone not needed because show() is already within semaphore lock.
    // takeLVGLSemaphore();
    update();
    // giveLVGLSemaphore();

    bindKeys();
}

void ButtonsWindow::addButton(std::string caption)
{
    buttons.push_back(caption);

    if (buttons.size() == 0)
    {
        selectedButtonIndex = -1;
    }
    else
    {
        selectedButtonIndex = 0;
    }
}

/**
 * @brief Method update() has to be called within takeLVGLSemaphore() and giveLVGLSemaphore().
 *
 */
void ButtonsWindow::update()
{
    // Get the current screen
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    // Create a Label on the currently active screen (only once)
    if (!pLabelMessage)
    {
        pLabelMessage = lv_label_create(scr, NULL);
        // Modify the Label's text
        lv_label_set_text(pLabelMessage, msg);
        lv_label_set_long_mode(pLabelMessage, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
        lv_label_set_align(pLabelMessage, captionAlign);       /*Center aligned lines*/
        lv_obj_set_width(pLabelMessage, 230);
        lv_obj_set_height(pLabelMessage, 180);
        lv_obj_align(pLabelMessage, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

        // pBtnsContainer = lv_cont_create(scr, NULL);
        // lv_obj_set_width(pBtnsContainer, 180);
        // lv_obj_set_height(pBtnsContainer, 50);
        // lv_obj_align(pBtnsContainer, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
        // lv_obj_set_auto_realign(pBtnsContainer, true);                    /*Auto realign when the size changes*/
        // lv_cont_set_fit(pBtnsContainer, LV_FIT_TIGHT);
        // lv_cont_set_layout(pBtnsContainer, LV_LAYOUT_ROW_MID);


        // TODO: use lv_btnmatrix in future
        int n = buttons.size();
        int w = 80;
        int h = 30;
        // int a = 240/(n + 1);
        int dw = (240 - n*w)/(n+1);

        for (int i=0; i<n; i++ ) 
        {
            lv_obj_t * pBtn = lv_btn_create(scr, NULL);
            lv_obj_add_style(pBtn, LV_OBJ_PART_MAIN, &style_btn_pressed);
            lv_obj_set_size(pBtn, w, h);
            lv_obj_align(pBtn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, dw + i*(dw + w) , -5);
            lv_obj_t * label = lv_label_create(pBtn, NULL);
            lv_label_set_text(label, buttons[i].c_str());

            pButtons.push_back(pBtn);
            if (i == 0)
                lv_btn_set_state(pBtn, LV_BTN_STATE_PRESSED);

        }
    }
}

void ButtonsWindow::keyEventsTask(void *arg)
{
    ButtonsWindow *sbw = (ButtonsWindow *)arg;
    sbw->activeTasks++;
    KeyActions ka;
    while (!sbw->closing)
    {
        if (xQueueReceive(sbw->keysQueue, &ka, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Key%d action received", ka.id);
            if (ka.id == 3 && ka.action == KEY_SHORT_PRESS)
            {
                // scroll down

                int ind = sbw->selectedButtonIndex;
                int newind = (sbw->selectedButtonIndex + 1) % sbw->buttons.size();
                lv_obj_t* btn = sbw->pButtons[ind];
                lv_obj_t* newbtn = sbw->pButtons[newind];
                sbw->selectedButtonIndex = newind;

                takeLVGLSemaphore();
                lv_btn_set_state(btn, LV_BTN_STATE_RELEASED);
                lv_btn_set_state(newbtn, LV_BTN_STATE_PRESSED);
                giveLVGLSemaphore();
            }
            else if (ka.id == 1 && ka.action == KEY_SHORT_PRESS)
            {
                sbw->close();
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
    sbw->activeTasks--;
    ESP_LOGI(TAG, "sbw_process_keys exits.");
    vTaskDelete(NULL);
}
