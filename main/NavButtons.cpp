
static const char *TAG = "NavButtons";

#include "esp_log.h"
#include "driver/gpio.h"
#include "NavButtons.h"

gpio_num_t gpios[3] = {MY_KEY1, MY_KEY2, MY_KEY3};

void config_my_buttons()
{
    ESP_LOGI(TAG, "Configuring buttons!");
}

NavButtons::NavButtons() : b1(1, MY_KEY1, this),
                           b2(2, MY_KEY2, this),
                           b3(3, MY_KEY3, this)
{
    if (!b1.init())
        ESP_LOGE(TAG, "Could not initialize b1.");
    if (!b2.init())
        ESP_LOGE(TAG, "Could not initialize b2.");
    if (!b3.init())
        ESP_LOGE(TAG, "Could not initialize b3.");
}

NavButtons::~NavButtons()
{
    b1.close();
    b2.close();
    b3.close();
}

void NavButtons::setKeyQueue(QueueHandle_t xQueue)
{
    b1.setKeyQueue(xQueue);
    b2.setKeyQueue(xQueue);
    b3.setKeyQueue(xQueue);
}

