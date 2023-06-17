#include "DisplayQRWindow.h"
#include "Button.h"
#include "GUI.h"

static const char *TAG = "DisplayQRWindow";

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
#include "qrcodegen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#define CANVAS_WIDTH 180
#define CANVAS_HEIGHT 180
#define QR_SIZE 240

// static lv_style_t style_black;
lv_obj_t *qrcode_create(lv_obj_t *parent, lv_color_t dark_color, lv_color_t light_color);
lv_res_t qrcode_update(lv_obj_t *canvas, const void *data, uint32_t data_len);

DisplayQRWindow::DisplayQRWindow(NavButtons *nb, std::string qrData) : Window("DisplayQRWindow"), KeyBinder(nb), qrData(qrData)
{
    // takeLVGLSemaphore();
    // lv_style_init(&style_black);
    // lv_style_set_bg_color(&style_black, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    // lv_style_set_image_recolor(&style_black, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    // giveLVGLSemaphore();
}
void DisplayQRWindow::createKeyEventsTask()
{
    xTaskCreatePinnedToCore(keyEventsTask, "keyEventsTask", 2048, this, 5, NULL, 1);
}


void DisplayQRWindow::show()
{
    ESP_LOGI(TAG, "show.");
    update();
    bindKeys();

    //    xTaskCreatePinnedToCore(updateTask, "update", 3 * 1024, this, 6, NULL, 1);
    // TODO: setup a timer to create dynamic QR codes for long data
}

void DisplayQRWindow::close()
{
    ESP_LOGI(TAG, "closing view.");
    Window::close();
    KeyBinder::unbindKeys();
}

void DisplayQRWindow::update()
{
    ESP_LOGI(TAG, "update");
    ESP_LOGI(TAG, "update with %s", qrData.c_str());
    // Initialize lvgl components on the currently active screen (only once)
    if (!pLabelCaption)
    {
        // Get the current screen
        lv_obj_t *scr = lv_disp_get_scr_act(NULL);
        pLabelCaption = lv_label_create(scr, NULL);
        // lv_label_set_long_mode(pLabelCaption, LV_LABEL_LONG_BREAK);
        lv_label_set_text(pLabelCaption, caption);
        lv_obj_set_width(pLabelCaption, 200);
        lv_obj_align(pLabelCaption, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        canvas = qrcode_create(scr, LV_COLOR_BLACK, LV_COLOR_WHITE);
        lv_res_t res = qrcode_update(canvas, (void *)qrData.c_str(), qrData.size());
    }

    if (pLabelCaption)
    {
        lv_res_t res = qrcode_update(canvas, (void *)qrData.c_str(), qrData.size());
    }
}

void DisplayQRWindow::keyEventsTask(void *arg)
{
    DisplayQRWindow *dqv = (DisplayQRWindow *)arg;
    dqv->activeTasks++;
    KeyActions ka;
    while (!dqv->closing)
    {
        if (xQueueReceive(dqv->keysQueue, &ka, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Key%d action received", ka.id);
            if (ka.id == 1 && ka.action == KEY_SHORT_PRESS)
            {
                dqv->close();
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
    dqv->activeTasks--;
    ESP_LOGI(TAG, "sqv_process_keys exits.");
    vTaskDelete(NULL);
}

static lv_color_t dark_color_param;
static lv_color_t light_color_param;
/*Create colors with the indices of the palette*/
static lv_color_t c0;
static lv_color_t c1;

lv_obj_t *qrcode_create(lv_obj_t *parent, lv_color_t dark_color, lv_color_t light_color)
{
    LV_LOG_INFO("begin");
    light_color_param = light_color;
    dark_color_param = dark_color;

    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(QR_SIZE, QR_SIZE)];
    /*Create a canvas and initialize its the palette*/
    lv_obj_t *canvas = lv_canvas_create(parent, NULL);
    lv_canvas_set_buffer(canvas, cbuf, QR_SIZE, QR_SIZE, LV_IMG_CF_INDEXED_1BIT);
    lv_canvas_set_palette(canvas, 1, light_color);
    lv_canvas_set_palette(canvas, 0, dark_color);
    c0.full = 0;
    c1.full = 1;

    lv_obj_align(canvas, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_fill_bg(canvas, c0, LV_OPA_COVER);
    return canvas;
}
lv_res_t qrcode_update(lv_obj_t *canvas, const void *data, uint32_t data_len)
{
    lv_color_t c;
    c.full = 1;
    lv_canvas_fill_bg(canvas, c, LV_OPA_COVER);

    if (data_len > qrcodegen_BUFFER_LEN_MAX)
        return LV_RES_INV;

    lv_img_dsc_t *imgdsc = lv_canvas_get_img(canvas);

    int32_t qr_version = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, data_len);
    if (qr_version <= 0)
        return LV_RES_INV;
    int32_t qr_size = qrcodegen_version2size(qr_version);
    if (qr_size <= 0)
        return LV_RES_INV;
    int32_t scale = imgdsc->header.w / qr_size;
    if (scale <= 0)
        return LV_RES_INV;
    int32_t remain = imgdsc->header.w % qr_size;

    /* The qr version is incremented by four point */
    uint32_t version_extend = remain / (scale << 2);
    if (version_extend && qr_version < qrcodegen_VERSION_MAX)
    {
        qr_version = qr_version + version_extend > qrcodegen_VERSION_MAX ? qrcodegen_VERSION_MAX : qr_version + version_extend;
    }

    uint8_t *qr0 = (uint8_t *)lv_mem_alloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_NULL(qr0);
    uint8_t *data_tmp = (uint8_t *)lv_mem_alloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_NULL(data_tmp);
    _lv_memcpy(data_tmp, data, data_len);

    bool ok = qrcodegen_encodeBinary(data_tmp, data_len,
                                     qr0, qrcodegen_Ecc_MEDIUM,
                                     qr_version, qr_version,
                                     qrcodegen_Mask_AUTO, true);

    if (!ok)
    {
        lv_mem_free(qr0);
        lv_mem_free(data_tmp);
        return LV_RES_INV;
    }

    lv_coord_t obj_w = imgdsc->header.w;
    qr_size = qrcodegen_getSize(qr0);
    scale = obj_w / qr_size;
    int scaled = qr_size * scale;
    int margin = (obj_w - scaled) / 2;
    uint8_t *buf_u8 = (uint8_t *)imgdsc->data + 8; /*+8 skip the palette*/

    /* Copy the qr code canvas:
     * A simple `lv_canvas_set_px` would work but it's slow for so many pixels.
     * So buffer 1 byte (8 px) from the qr code and set it in the canvas image */
    uint32_t row_byte_cnt = (imgdsc->header.w + 7) >> 3;
    int y;
    for (y = margin; y < scaled + margin; y += scale)
    {
        uint8_t b = 0;
        uint8_t p = 0;
        bool aligned = false;
        int x;
        for (x = margin; x < scaled + margin; x++)
        {
            bool a = qrcodegen_getModule(qr0, (x - margin) / scale, (y - margin) / scale);

            if (aligned == false && (x & 0x7) == 0)
                aligned = true;

            if (aligned == false)
            {
                c.full = a ? 0 : 1;
                lv_canvas_set_px(canvas, x, y, c);
            }
            else
            {
                if (!a)
                    b |= (1 << (7 - p));
                p++;
                if (p == 8)
                {
                    uint32_t px = row_byte_cnt * y + (x >> 3);
                    buf_u8[px] = b;
                    b = 0;
                    p = 0;
                }
            }
        }

        /*Process the last byte of the row*/
        if (p)
        {
            /*Make the rest of the bits white*/
            b |= (1 << (8 - p)) - 1;

            uint32_t px = row_byte_cnt * y + (x >> 3);
            buf_u8[px] = b;
        }

        /*The Qr is probably scaled so simply to the repeated rows*/
        int s;
        const uint8_t *row_ori = buf_u8 + row_byte_cnt * y;
        for (s = 1; s < scale; s++)
        {
            _lv_memcpy((uint8_t *)buf_u8 + row_byte_cnt * (y + s), row_ori, row_byte_cnt);
        }
    }

    lv_mem_free(qr0);
    lv_mem_free(data_tmp);
    return LV_RES_OK;
}
