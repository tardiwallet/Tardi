#include "ScanQRWindow.h"
#include "Button.h"
#include "GUI.h"
#include "Camera.h"
#include "CodeScanner.h"
#include <sstream>

static const char *TAG = "ScanQRWindow";

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "esp_system.h"
#include "esp_heap_caps.h"

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

static lv_style_t style_black;

static camera_fb_t local_fb = {NULL, 0, 0, 0, PIXFORMAT_GRAYSCALE, {0, 0}};
static SemaphoreHandle_t local_fb_mutex = xSemaphoreCreateMutex();
// static TaskHandle_t decodeTaskHandle;

#define CANVAS_WIDTH  240
#define CANVAS_HEIGHT  180
static lv_color_t cbuf[LV_IMG_BUF_SIZE_INDEXED_8BIT(CANVAS_WIDTH, CANVAS_HEIGHT)];

ScanQRWindow::ScanQRWindow(NavButtons *nb) : Window("ScanQRWindow"), KeyBinder(nb)
{
    decodeQueue = xQueueCreate(3, sizeof(uint32_t));

    takeLVGLSemaphore();
    lv_style_init(&style_black);
    lv_style_set_bg_color(&style_black, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&style_black, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    giveLVGLSemaphore();

    pCanvas = NULL;
}

void ScanQRWindow::createKeyEventsTask()
{
    xTaskCreatePinnedToCore(keyEventsTask, "keyEventsTask", 2048, this, 5, NULL, 1);
}

void ScanQRWindow::show()
{
    ESP_LOGI(TAG, "show.");

    // take and give semaphone not needed because show() is already within semaphore lock.
    // takeLVGLSemaphore();
    update();
    // giveLVGLSemaphore();

    bindKeys();

    xTaskCreatePinnedToCore(decodeTask, "scanner", 6 * 1024, this, 6, NULL, 1);
    xTaskCreatePinnedToCore(updateTask, "update", 3 * 1024, this, 6, NULL, 1);
    // eTaskGetState(decodeTaskHandle);
}

void ScanQRWindow::close()
{
    ESP_LOGI(TAG, "closing view.");
    Window::close();
    KeyBinder::unbindKeys();
}

/**
 * @brief Method update() has to be called within takeLVGLSemaphore() and giveLVGLSemaphore().
 *
 */
void ScanQRWindow::update()
{
    ESP_LOGI(TAG, "update.");
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    // Initialize lvgl components on the currently active screen (only once)
    if (!pLabelCaption)
    {
        // Get the current screen
        pLabelCaption = lv_label_create(scr, NULL);
        // lv_label_set_long_mode(pLabelCaption, LV_LABEL_LONG_BREAK);
        lv_label_set_text(pLabelCaption, caption);
        lv_obj_set_width(pLabelCaption, 200);
        lv_obj_align(pLabelCaption, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        pLabelProgress = lv_label_create(scr, NULL);
        lv_label_set_text(pLabelProgress, "");
        lv_obj_set_width(pLabelProgress, 200);
        lv_obj_align(pLabelProgress, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

        pCanvas = lv_canvas_create(scr, NULL);
        lv_canvas_set_buffer(pCanvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_INDEXED_8BIT);
        lv_obj_set_width(pCanvas, CANVAS_WIDTH);
        lv_obj_set_height(pCanvas, CANVAS_HEIGHT);
        lv_obj_align(pCanvas, NULL, LV_ALIGN_CENTER, 0, 5);
        // lv_obj_align(pCanvas, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 20);
        for(int i=0; i<256;i++)
            lv_canvas_set_palette(pCanvas, i, lv_color_make(i, i, i));
        // lv_canvas_fill_bg(pCanvas, LV_COLOR_WHITE, LV_OPA_COVER);

        /*Set a very visible color for the screen to clearly see what happens*/
        // lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex3(0xf33));
    }

    if (pLabelProgress)
    {
        std::ostringstream label;
        label << "scanned " << qrCodes.size() << " out of N";
        lv_label_set_text(pLabelProgress, label.str().c_str());
        lv_obj_set_width(pLabelProgress, 200);
        lv_obj_align(pLabelProgress, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    }

}

static void free_local_fb()
{
    if (xSemaphoreTake(local_fb_mutex, 0) == pdTRUE)
    {
        if (local_fb.len != 0)
        {
            free(local_fb.buf);
            local_fb.len = 0;
            local_fb.buf = NULL;
        }
        xSemaphoreGive(local_fb_mutex);
    }
}

bool copy_fb(camera_fb_t *dst, camera_fb_t *src)
{
    if (xSemaphoreTake(local_fb_mutex, 0) != pdTRUE)
    {
        if (dst && src)
        {
            dst->format = src->format;
            dst->height = src->height;
            dst->width = src->width;
            dst->buf = (uint8_t *)malloc(src->len);
            if (dst->buf)
            {
                memcpy(dst->buf, src->buf, src->len);
                dst->len = src->len;
                xSemaphoreGive(local_fb_mutex);
                return true;
            }
            else
            {
                dst->len = 0;
                xSemaphoreGive(local_fb_mutex);
                return false;
            }
        }
        else
        {
            xSemaphoreGive(local_fb_mutex);
            return false;
        }
    }
    else
        return false;
}

void ScanQRWindow::decodeTask(void *arg)
{
    ScanQRWindow *sqv = (ScanQRWindow *)arg;
    sqv->activeTasks++;
    Camera camera;

    if (camera.init())
    {
        CodeScanner scanner;
        vTaskDelay(pdMS_TO_TICKS(1000));
        while (!sqv->closing)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            camera_fb_t *fb = camera.get_fb();
            if (fb)
            {
                free_local_fb();
                copy_fb(&local_fb, fb);
                camera.return_fb();
                if (xSemaphoreTake(local_fb_mutex, 0) == pdTRUE)
                {
                    ESP_LOGD(TAG, "Reading local buffer (Mutex obtained) from 0x%p", (void *)local_fb.buf);
                    if (local_fb.buf != NULL && scanner.decode(&local_fb))
                    {
                        ESP_LOGI(TAG, "QR Scanner found something ...");
                        int decodeCount = scanner.getDecodeCount();
                        if (decodeCount > 0)
                        {
                            std::vector<CodeScannerResult> results = scanner.getResults();
                            ESP_LOGI(TAG, "Decode time in %lld ms.", scanner.getDecodeTimeMs());
                            for (int i = 0; i < results.size(); i++)
                            {
                                sqv->qrCodes.push_back(results[i].data);
                                ESP_LOGI(TAG, "Decoded %s symbol \"%s\"\n", results[i].type_name.c_str(), results[i].data.c_str());
                            }
                            xQueueSend(sqv->decodeQueue, &decodeCount, 0);
                        }
                        else
                            ESP_LOGI(TAG, "No code detected.");
                    }

                    takeLVGLSemaphore();
                    ESP_LOGD(TAG, "Drawing on canvas");
                    lv_canvas_fill_bg(sqv->pCanvas, LV_COLOR_WHITE, LV_OPA_100);
                    static lv_img_dsc_t my_img_dsc;
                    my_img_dsc.header.always_zero = 0;
                    my_img_dsc.header.w = local_fb.width;
                    my_img_dsc.header.h = local_fb.height;
                    my_img_dsc.data_size = local_fb.len;
                    my_img_dsc.header.cf = LV_IMG_CF_INDEXED_8BIT;          /*Set the color format*/
                    my_img_dsc.data = local_fb.buf + 256;
                    lv_canvas_transform(sqv->pCanvas, &my_img_dsc, 0, 96, 0, 0, 0, 0, true);
                    giveLVGLSemaphore();

                    xSemaphoreGive(local_fb_mutex);
                }
            }
            else
                ESP_LOGI(TAG, "Reading camera  failed. Framebuffer NULL");


        }
    }
    sqv->activeTasks--;
    ESP_LOGI(TAG, "decodeTask exits.");
    vTaskDelete(NULL);
}

void ScanQRWindow::updateTask(void *arg)
{
    ScanQRWindow *sqv = (ScanQRWindow *)arg;
    sqv->activeTasks++;
    vTaskDelay(pdMS_TO_TICKS(1000));
    while (!sqv->closing)
    {
        int decodeCount = 0;
        if (xQueueReceive(sqv->decodeQueue, &decodeCount, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            takeLVGLSemaphore();
            sqv->update();
            giveLVGLSemaphore();
        }
    }
    sqv->activeTasks--;
    ESP_LOGI(TAG, "updateTask exits.");
    vTaskDelete(NULL);
}

void ScanQRWindow::keyEventsTask(void *arg)
{
    ScanQRWindow *sqv = (ScanQRWindow *)arg;
    sqv->activeTasks++;
    KeyActions ka;
    while (!sqv->closing)
    {
        if (xQueueReceive(sqv->keysQueue, &ka, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Key%d action received", ka.id);
            if (ka.id == 1 && ka.action == KEY_SHORT_PRESS)
            {
                sqv->close();
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
    sqv->activeTasks--;
    ESP_LOGI(TAG, "sqv_process_keys exits.");
    vTaskDelete(NULL);
}

