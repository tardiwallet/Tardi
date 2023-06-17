#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

    static const char *TAG = "CodeScanner";

#ifdef __cplusplus
}
#endif

#include "CodeScanner.h"

CodeScanner::CodeScanner()
{
}

CodeScanner::~CodeScanner()
{
    cleanUpResults();
}

bool CodeScanner::decode(camera_fb_t *fb)
{
    cleanUpResults();

    if (fb == NULL || fb->buf == NULL) 
    {
        ESP_LOGE(TAG, "Framebuffer is null.");
        return false;
    }

    // esp_code_scanner_config_t config = {ESP_CODE_SCANNER_MODE_FAST, ESP_CODE_SCANNER_IMAGE_RGB565, fb->width, fb->height};
    esp_code_scanner_config_t config = {ESP_CODE_SCANNER_MODE_FAST, ESP_CODE_SCANNER_IMAGE_GRAY, fb->width, fb->height};

    esp_image_scanner_t *esp_scn = esp_code_scanner_create();
    if (!esp_scn)
    {
        ESP_LOGE(TAG, "Could not create scanner.");
        return false;
    }
    if (esp_code_scanner_set_config(esp_scn, config) == ESP_FAIL)
    {
        ESP_LOGE(TAG, "Failed to config scanner.");
        esp_code_scanner_destroy(esp_scn);
        return false;
    }

    ESP_LOGD(TAG, "Scanning ...");
    int64_t startTime = esp_timer_get_time();
    int decodedCount = esp_code_scanner_scan_image(esp_scn, fb->buf);
    if (!decodedCount)
    {
        ESP_LOGD(TAG, "Nothing found.");
        esp_code_scanner_destroy(esp_scn);
        return false;
    }

    ESP_LOGD(TAG, "Decoding ...");
    esp_code_scanner_symbol_t results = esp_code_scanner_result(esp_scn);
    decodeTimeMs = (esp_timer_get_time() - startTime) / 1000;
    ESP_LOGI(TAG, "Results decoded in %u ms", (uint32_t)decodeTimeMs);

    decodedResults.push_back(CodeScannerResult(results.type_name,results.data));
    esp_code_scanner_symbol_t* next = results.next;
    esp_code_scanner_symbol_t* tmp = NULL;
    while (next)
    {
        decodedResults.push_back(CodeScannerResult(next->type_name, next->data));
        tmp = next;
        next = next->next;
        free(tmp);
    }
    esp_code_scanner_destroy(esp_scn);
    return true;
}

void CodeScanner::cleanUpResults()
{
    ESP_LOGD(TAG, "Cleanup existing results.");
    decodedResults.clear();
}

void freeDecodedResults(esp_code_scanner_symbol_t *head)
{
    esp_code_scanner_symbol_t *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}