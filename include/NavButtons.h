#pragma once

#define MY_KEY3 GPIO_NUM_26
#define MY_KEY2 GPIO_NUM_32
#define MY_KEY1 GPIO_NUM_33

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#include "Button.h"

class NavButtons
{
private:
    Button b1;
    Button b2;
    Button b3;

public:
    NavButtons();
    ~NavButtons();
    void setKeyQueue(QueueHandle_t xQueue);
};

