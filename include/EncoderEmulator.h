#ifndef TARDI_ENCODEREMULATOR_H_
#define TARDI_ENCODEREMULATOR_H_

#ifdef __cplusplus
extern "C"
{
#endif

// #include "driver/gpio.h"
// #include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "freertos/queue.h"
// #include "freertos/timers.h"
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#include "GpioKey.h"
#include "Debouncer.h"

typedef enum
{
    ENCODER_PRESSED = 0,
    ENCODER_RELEASED = 1,
    ENCODER_NEXT = 2,
    ENCODER_PREV = 3,
} EncoderState;


class EncoderEmulator
{
private:
    GpioKey* key1 = nullptr;
    GpioKey* key3 = nullptr;
    Debouncer<GpioKeyState>* db1 = nullptr;
    Debouncer<GpioKeyState>* db3 = nullptr;

    lv_indev_t * indevEncoder;
    static bool encoderRead(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

    TaskHandle_t mTaskHandle = nullptr;
    static void keyScanTask(void *arg);
public:
    EncoderEmulator();
    ~EncoderEmulator();
    lv_indev_t * getIndev() const { return indevEncoder; }
};


#endif  // TARDI_ENCODEREMULATOR_H_