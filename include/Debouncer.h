#ifndef TARDI_DEBOUNCER_H_
#define TARDI_DEBOUNCER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif
template <typename T>
class Debouncer
{
private:
    static constexpr const char* TAG = "Debouncer";
    int mDebounceTimeMs;
    QueueHandle_t mInputQueueHandle = nullptr;
    QueueHandle_t mOutputQueueHandle = nullptr;
    TaskHandle_t mTaskHandle = nullptr;
    TimerHandle_t mTimer = nullptr;
    T mLastState;
    T mCandidateState;
    static void queueReadTask(void *arg)
    {
        Debouncer<T> *db = (Debouncer<T> *)arg;
        T data;
        while (true)
        {
            if (xQueueReceive(db->mInputQueueHandle, &data, pdMS_TO_TICKS(db->mDebounceTimeMs/5)) == pdTRUE)
            { // pdMS_TO_TICKS(100) portMAX_DELAY
                ESP_LOGD(TAG, "Data received on input queue. %d", data);
                if (data != db->mLastState) 
                {
                    db->mCandidateState = data;
                    if (xTimerIsTimerActive(db->mTimer) == pdFALSE )
                        assert(xTimerStart(db->mTimer, 0 ) == pdPASS);
                    else
                        assert(xTimerReset(db->mTimer, 0 ) == pdPASS);
                }
            }
        }

        vTaskDelete(NULL);
    }
    static void timerCallback(TimerHandle_t pxTimer)
    {
        Debouncer<T> *db = (Debouncer<T> *)pvTimerGetTimerID(pxTimer);

        assert(xQueueSend(db->mOutputQueueHandle, &(db->mCandidateState), 0) == pdTRUE);
        db->mLastState = db ->mCandidateState;
        ESP_LOGD(TAG, "Data debounced.");
    }

public:
    Debouncer(int debounceTimeMs, QueueHandle_t inputQueueHandle, T initState): 
        mDebounceTimeMs(debounceTimeMs), 
        mInputQueueHandle(inputQueueHandle),
        mLastState(initState),
        mCandidateState(initState)
    {
        assert(mDebounceTimeMs > 0);
        assert(mInputQueueHandle);
        mOutputQueueHandle = xQueueCreate(5, sizeof(T));
        assert(mOutputQueueHandle);

        assert(xTaskCreate(queueReadTask, "queueReadTask", 2048, this, 5, &(this->mTaskHandle)) == pdPASS);
        mTimer = xTimerCreate("DebounceTimer", pdMS_TO_TICKS(mDebounceTimeMs), pdFALSE, this, timerCallback);
        assert(mTimer);
        ESP_LOGD(TAG, "Debouncer starts.");
    }
    ~Debouncer()
    {
        if (mTaskHandle)
        {
            vTaskDelete(mTaskHandle);
            mTaskHandle = nullptr;
        }
        if (mOutputQueueHandle)
        {
            assert(xQueueReset(mOutputQueueHandle) == pdPASS);
            vQueueDelete(mOutputQueueHandle);
            mOutputQueueHandle = nullptr;
        }
        if (mTimer)
        {
            assert(xTimerDelete(mTimer, 0) == pdPASS);
            mTimer = nullptr;
        }    
    }

    int getDebounceTimeInMs() const { return mDebounceTimeMs; };
    QueueHandle_t getOutputQueueHandle() const { return mOutputQueueHandle; };
};

#endif // DEBOUNCER