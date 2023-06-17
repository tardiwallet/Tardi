#ifdef __cplusplus
extern "C"
{
#endif

static const char *TAG = "GpioKey";

#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#include "GpioKey.h"

inline bool install_isr_service()
{
    esp_err_t result = gpio_install_isr_service(0);
    if (result != ESP_OK && result != ESP_ERR_INVALID_STATE)
        return false;
    return true;
}

static void IRAM_ATTR gpio_isr_handler_key(void *arg)
{
    GpioKey *key = (GpioKey *)arg;
    GpioKeyState state = gpio_get_level(key->getGpioNum()) == 1 ? GPIOKEY_RELEASED : GPIOKEY_PRESSED;
    /* We have not woken a task at the start of the ISR. */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(key->getQueueHandle(), &state, &xHigherPriorityTaskWoken);
}

GpioKey::GpioKey(gpio_num_t gpioNum): mGpioNum(gpioNum)
{
    // initialize the gpio and add its handler
    gpio_config_t io_conf = {(uint64_t)1LL << mGpioNum, GPIO_MODE_INPUT, GPIO_PULLUP_ENABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_ANYEDGE};
    gpio_config(&io_conf);

    mQueueHandle = xQueueCreate(5, sizeof(GpioKeyState));
    assert(mQueueHandle);
    assert(install_isr_service());
    assert(gpio_isr_handler_add(mGpioNum, gpio_isr_handler_key, (void *)this) == ESP_OK);

    ESP_LOGD(TAG, "GpioKey initialized");
}

GpioKey::~GpioKey()
{
    gpio_isr_handler_remove(mGpioNum);
    if (mQueueHandle)
    {
        xQueueReset(mQueueHandle);
        vQueueDelete(mQueueHandle);
        mQueueHandle = nullptr;
    }
}

