#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

typedef enum
{
    KEY_SHORT_PRESS = 1,
    KEY_LONG_PRESS,
    KEY_DOUBLE_CLICK,
} key_state_t;

class Button
{
private:
    int id;
    bool initialized;
    int lvgl_state;
    gpio_num_t gpio_num;
    void *arg;
    QueueHandle_t gpio_evt_queue = NULL;
    int64_t last_time = 0;
    QueueHandle_t xKeyQueue = NULL;
    TaskHandle_t xHandle = NULL;

    int process_queue();

    static void key_scan_task(void *arg);
    static void debounce_timer_callback(TimerHandle_t xTimer);


public:
    TimerHandle_t debounce_timer = NULL;
    Button(int id, gpio_num_t gpio_num, void *arg);
    ~Button();

    bool init();
    void close();
    bool isInitialized() { return initialized; };
    void raise_event();
    int get_id() { return id; }
    gpio_num_t get_gpio_num() { return gpio_num; }
    xQueueHandle get_gpio_evt_queue() { return gpio_evt_queue; }
    void setKeyQueue(QueueHandle_t xKeyQueue);
    int get_lvgl_state() { return lvgl_state; }
    void set_lvgl_state(int lvgl_state) { this->lvgl_state = lvgl_state; }
    void *get_arg() { return arg; }
};

struct KeyActions
{
    int id;
    int action;
};
