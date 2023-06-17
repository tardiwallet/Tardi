#ifndef TARDI_GPIOKEY_H_
#define TARDI_GPIOKEY_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include "driver/gpio.h"
#include "freertos/queue.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

typedef enum
{
    GPIOKEY_PRESSED = 0,
    GPIOKEY_RELEASED = 1,
} GpioKeyState;

class GpioKey
{
private:
    gpio_num_t mGpioNum = GPIO_NUM_NC;
    QueueHandle_t mQueueHandle = nullptr;
public:
    GpioKey(gpio_num_t gpioNum);
    ~GpioKey();

    gpio_num_t getGpioNum() const { return mGpioNum; }
    QueueHandle_t getQueueHandle() const { return mQueueHandle; }
};

#endif  // TARDI_GPIOKEY_H_