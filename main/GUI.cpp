#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "lvgl_helpers.h"


static const char *TAG = "GUI";

#ifdef __cplusplus
}
#endif

#include "GUI.h"
#include "Window.h"

#define LV_TICK_PERIOD_MS 1
#define GUI_TASK_PRIORITY 3

static void lv_tick_callback(void *arg);
static void gui_task(void *arg);

static TaskHandle_t guiTaskHandler;
static esp_timer_handle_t lvTickTimer;
/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
static SemaphoreHandle_t xGuiSemaphore;
static lv_disp_buf_t disp_buf;
static lv_color_t *buf1;
static lv_color_t *buf2;

GUI::GUI(BaseType_t core)
{
    taskCore = core;
    initialized = false;
    currentView = NULL;
}

GUI::~GUI()
{
    if (initialized)
    {
        closeView();
        esp_timer_stop(lvTickTimer);
        esp_timer_delete(lvTickTimer);
        vTaskDelete(guiTaskHandler);
        free(buf1);
        free(buf2);
    }
}

bool GUI::init()
{
    if (initialized)
    {
        ESP_LOGE(TAG, "Already initialized.");
        return false;
    }

    xGuiSemaphore = xSemaphoreCreateMutex();
    if (!xGuiSemaphore)
    {
        ESP_LOGE(TAG, "Could not create Mutex.");
        return false;
    }

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (buf1 == NULL || buf2 == NULL)
    {
        ESP_LOGE(TAG, "Could not create display buffers.");
        assert(buf1 != NULL);
        assert(buf2 != NULL);
        return false;
    }

    uint32_t size_in_px = DISP_BUF_SIZE;

    ESP_LOGD(TAG, "Initializing the working buffer.");
    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    ESP_LOGD(TAG, "Initializing display driver.");
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    ESP_LOGD(TAG, "Registering display driver.");
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    ESP_LOGI(TAG, "Creating and starting a periodic timer interrupt to call lv_tick_inc.");
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_callback,
        .name = "periodic_gui"};
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &lvTickTimer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvTickTimer, LV_TICK_PERIOD_MS * 1000));

    ESP_LOGD(TAG, "Staring the gui tasks.");
    xTaskCreatePinnedToCore(gui_task, "guiTask", 8 * 1024, this, GUI_TASK_PRIORITY, &guiTaskHandler, taskCore);
    initialized = true;
    return initialized;
}

void GUI::showView(Window *view)
{
    if (initialized)
    {
        closeView();
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            ESP_LOGD(TAG, "Showing view %s.", view->name);
            view->show();
            this->currentView = view;
            xSemaphoreGive(xGuiSemaphore);
        }
        update();
    }
}

void GUI::closeView()
{
    if (!initialized)
        return;
    if (currentView)
    {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            ESP_LOGD(TAG, "Closing view %s.", currentView->name);
            currentView->close();
            currentView = NULL;
            // Get the current screen
            lv_obj_t *scr = lv_disp_get_scr_act(NULL);
            lv_obj_clean(scr);
            xSemaphoreGive(xGuiSemaphore);
            ESP_LOGD(TAG, "View Closed.");
        }
        update();
    }
}

void GUI::waitToClose(Window *view)
{
    if (!initialized)
        return;
    if (view)
    {
        while (!view->isClosed())
            vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void GUI::update()
{
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
    {
        ESP_LOGV(TAG, "Update");
        lv_task_handler();
        xSemaphoreGive(xGuiSemaphore);
    }
}


static void lv_tick_callback(void *arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void gui_task(void *arg)
{
    GUI *gui = (GUI *)arg;
    ESP_LOGD(TAG, "gui_task starts");

    uint32_t time_to_next_call = 10;
    while (true)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(time_to_next_call));
        gui->update();
    }

    vTaskDelete(NULL);
    ESP_LOGD(TAG, "gui_task ends");
}

void takeLVGLSemaphore()
{
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
}

void giveLVGLSemaphore()
{
    xSemaphoreGive(xGuiSemaphore);
    // LV_INDEV_STATE_PRESSED;
}


