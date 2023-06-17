#include "Button.h"

#ifdef __cplusplus
extern "C"
{
#endif
    static const char *TAG = "Button";

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
}
#endif

const TickType_t debouce_ticks = pdMS_TO_TICKS(10);

#define LONG_PRESS_THRESH 700000
#define DOUBLE_CLICK_THRESH 300000

static bool isr_service_installed = false;
static bool install_isr_service()
{
    if (isr_service_installed)
        return true;
    if (gpio_install_isr_service(0) != ESP_OK)
        return false;
    isr_service_installed = true;
    return true;
}

/**
 * Handles a key event to a Button. The function reads the Button gpio_num and sends the current gpio level to the gpio_evt_queue.
 * Additionally, it sets the Button lvgl_state to "LV_INDEV_STATE_PR" or "LV_INDEV_STATE_REL".
 */

static void IRAM_ATTR gpio_isr_handler_key(void *arg)
{
    Button *btn = (Button *)arg;
    xTimerReset(btn->debounce_timer, 0);
    // int lvl = gpio_get_level(btn->get_gpio_num());
    // btn->set_next_lvgl_state(lvl == 0 ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL);
    /* We have not woken a task at the start of the ISR. */
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xQueueSendFromISR(btn->get_gpio_evt_queue(), &lvl, &xHigherPriorityTaskWoken);
}

Button::Button(int id, gpio_num_t gpio_num, void *arg) : id(id), initialized(false),
                                                         lvgl_state(LV_INDEV_STATE_REL), gpio_num(gpio_num), arg(arg)
{
}

Button::~Button()
{
    close();
}

void Button::close()
{
    initialized = false;
    gpio_isr_handler_remove(gpio_num);
    if (xHandle)
    {
        vTaskDelete(xHandle);
        xHandle = NULL;
    }
    if (gpio_evt_queue)
    {
        xQueueReset(gpio_evt_queue);
        vQueueDelete(gpio_evt_queue);
        gpio_evt_queue = NULL;
    }
}

/**
 * Initializes the Button with the already given parameters from the constructor:
 * 1. configurates gpio
 * 2. creates gpio_evt_queue
 * 3. installs gpio ISR service
 * 4. adds ISR handler (gpio_isr_handler_key)
 * 5. creates a task to read the keys (key_scan_task)
 *
 * @return true if initialized correctly.
 */
bool Button::init()
{
    initialized = false;

    // initialize the gpio and add its handler
    gpio_config_t io_conf = {0};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = 1LL << gpio_num;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    if (!gpio_evt_queue)
        gpio_evt_queue = xQueueCreate(5, sizeof(uint32_t));
    if (!gpio_evt_queue)
        return false;
    if (!install_isr_service())
        return false;
    if (gpio_isr_handler_add(gpio_num, gpio_isr_handler_key, (void *)this) != ESP_OK)
        return false;
    // create a task on core 0 with prio 5 to frequently read the buttons
    // xQueueKeyStateO = key_state_o;
    xTaskCreatePinnedToCore(key_scan_task, "key_scan_task", 2048, this, 5, &(this->xHandle), 1);

    debounce_timer = xTimerCreate("btn_debounce", debouce_ticks, pdFALSE, (void *)this, debounce_timer_callback);
    // if (!debounce_timer)
    //     return false;
    initialized = true;
    ESP_LOGI(TAG, "Button%d initialized", id);
    return initialized;
}

/**
 * Task which calls key_scan function and executes the relevant callbacks assigned to Button.
 *
 * @param arg A pointer to the argument passed to the function.
 */
void Button::key_scan_task(void *arg)
{
    int ret = 0;
    Button *btn = (Button *)arg;
    vTaskDelay(pdMS_TO_TICKS(1000)); // wait to load and get initialized
    while (btn->initialized)
    {
        // ESP_LOGD(TAG, "key_scan_task on Button%d loop", btn->id);
        if (btn->xKeyQueue)
        {
            // ESP_LOGD(TAG, "process_queue on Button%d", btn->id);
            ret = btn->process_queue();
            KeyActions ka = {btn->id, ret};
            xQueueSend(btn->xKeyQueue, &ka, pdMS_TO_TICKS(100));
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    vTaskDelete(NULL);
}

/**
 * Processes data from gpio_evt_queue and returns key_state_t enum.
 *
 * @return int from key_state_t enum.
 */
int Button::process_queue()
{
    int gpio_level;
    BaseType_t press_key = pdFALSE;
    BaseType_t lift_key = pdFALSE;
    int64_t backup_time = 0;
    int64_t interval_time = 0;
    last_time = 0;

    while (initialized)
    {
        if (xQueueReceive(gpio_evt_queue, &gpio_level, pdMS_TO_TICKS(10)) != pdTRUE)
        { // pdMS_TO_TICKS(100) portMAX_DELAY
            continue;
        }
        ESP_LOGD(TAG, "Data received on queue for Button %d.", id);
        if (gpio_level == 0)
        {
            press_key = pdTRUE;
            backup_time = esp_timer_get_time();
            interval_time = backup_time - last_time;
        }
        else if (press_key)
        {
            lift_key = pdTRUE;
            last_time = esp_timer_get_time();
            backup_time = last_time - backup_time;
        }

        if (press_key & lift_key)
        {
            press_key = pdFALSE;
            lift_key = pdFALSE;

            if (backup_time > LONG_PRESS_THRESH)
            {
                ESP_LOGD(TAG, "Button %d, KEY_LONG_PRESS detected.", id);
                return KEY_LONG_PRESS;
            }
            else
            {
                if ((interval_time < DOUBLE_CLICK_THRESH) && (interval_time > 0))
                {
                    ESP_LOGD(TAG, "Button %d, KEY_DOUBLE_CLICK detected.", id);
                    return KEY_DOUBLE_CLICK;
                }
                else
                {
                    ESP_LOGD(TAG, "Button %d, KEY_SHORT_PRESS detected.", id);
                    return KEY_SHORT_PRESS;
                }
            }
        }
    }
    return 0;
}

void Button::setKeyQueue(QueueHandle_t xKeyQueue)
{
    this->xKeyQueue = xKeyQueue;
    ESP_LOGD(TAG, "xKeyQueue set to 0x%p for Button %d.", (void *)xKeyQueue, id);
}

void Button::debounce_timer_callback(TimerHandle_t xTimer)
{
    Button *btn = (Button *)pvTimerGetTimerID(xTimer);
    int lvl = gpio_get_level(btn->gpio_num);
    int final_lvgl_state = (lvl == 0 ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL);

    if (final_lvgl_state != btn->lvgl_state)
    {
        btn->set_lvgl_state(final_lvgl_state);
        xQueueSend(btn->get_gpio_evt_queue(), &lvl, 0);
    }
}
