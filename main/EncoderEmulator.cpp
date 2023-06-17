#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#include "EncoderEmulator.h"

EncoderEmulator::EncoderEmulator()
{
    lv_indev_drv_t indev_drv;

    key1 = new GpioKey(GPIO_NUM_33);
    key3 = new GpioKey(GPIO_NUM_26);

    db1 = new Debouncer<GpioKeyState>(50, key1->getQueueHandle(), GPIOKEY_RELEASED);
    db3 = new Debouncer<GpioKeyState>(50, key3->getQueueHandle(), GPIOKEY_RELEASED);    

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoderRead;
    indevEncoder = lv_indev_drv_register(&indev_drv);
    assert(indevEncoder);
    assert(xTaskCreate(keyScanTask, "keyScanTask", 2048, this, 5, &(this->mTaskHandle)) == pdPASS);

}

EncoderEmulator::~EncoderEmulator()
{
    delete db1;
    delete db3;
    delete key1;
    delete key3;
}

static int encoder_diff;
static lv_indev_state_t encoder_state;

bool EncoderEmulator::encoderRead(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    if (encoder_diff > 0)
    {
        data->enc_diff = encoder_diff;
        encoder_diff = 0;
    }
    data->state = encoder_state;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

void EncoderEmulator::keyScanTask(void *arg)
{
    EncoderEmulator *ee = (EncoderEmulator*)arg;
    GpioKeyState gpio_state;

    while(true)
    {
        if (xQueueReceive(ee->db1->getOutputQueueHandle(), &gpio_state, 0) == pdTRUE)
        {
            if (gpio_state == GPIOKEY_PRESSED)
                encoder_state = LV_INDEV_STATE_PR;
            if (gpio_state == GPIOKEY_RELEASED)
                encoder_state = LV_INDEV_STATE_REL;
        }
        if (xQueueReceive(ee->db3->getOutputQueueHandle(), &gpio_state, 0) == pdTRUE)
        {
            encoder_diff = gpio_state == GPIOKEY_PRESSED? 1: 0;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);

}

