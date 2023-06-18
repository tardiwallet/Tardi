/*
Tardi BitSigner app
*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"


#ifdef __cplusplus
}
#endif

// #include "examples/examples.h"
#include "Controller.h"
// #include "GpioKey.h"
// #include "Debouncer.h"
// #include "EncoderEmulator.h"
// #include "GUI.h"

// #include "test.h"
// #include <string>
// #include "json.hpp"
// #include <string>
// #include <vector>
// #include <iostream>
// #include <stdexcept>

// #include "params.hpp"
// #include "format.hpp"

// #include "sdkconfig.h"
// #include "esp_http_server.h"
// #include "quirc.h"
// #include "esp_heap_trace.h"
// #include "freertos/queue.h"

// void setup_wifi_ap(void);
// httpd_handle_t start_webserver(void);
// void stop_webserver(httpd_handle_t server);

static const char *TAG = "Main";

void set_log_level(esp_log_level_t level)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("Main", level);
    esp_log_level_set("CodeScanner", ESP_LOG_INFO);
    esp_log_level_set("Camera", ESP_LOG_INFO);
    esp_log_level_set("GUI", ESP_LOG_INFO);
    esp_log_level_set("SimpleButtonsWindow", level);
    esp_log_level_set("ListRotatorWindow", level);
    esp_log_level_set("MessageWindow", level);
    esp_log_level_set("ScanQRWindow", level);
    esp_log_level_set("DisplayQRWindow", level);
    esp_log_level_set("Button", level);
    esp_log_level_set("NavButtons", level);
    // esp_log_level_set("Debouncer", level);
    // esp_log_level_set("GpioKey", level);
}

#include "incl_test.h"


extern "C" void app_main(void)
{
    set_log_level(ESP_LOG_DEBUG);
#ifdef __cplusplus
    ESP_LOGI(TAG, "__cplusplus = %ld", __cplusplus);
#endif
    xTaskCreatePinnedToCore(controlTask, "controller", 4 * 1024, NULL, 2, NULL, 0);

    // xTaskCreatePinnedToCore(testKeys, "controller", 4 * 1024, NULL, 2, NULL, 0);
    // xTaskCreatePinnedToCore(testEncoderEmulator, "controller", 4 * 1024, NULL, 2, NULL, 0);

    ESP_LOGI(TAG, "bye bye!");
}
