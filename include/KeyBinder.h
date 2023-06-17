#pragma once

#include "NavButtons.h"

class KeyBinder
{
private:
protected:
    NavButtons *nb = NULL;
    QueueHandle_t keysQueue;

    virtual void createKeyEventsTask() = 0;

public:
    KeyBinder(NavButtons *nb) : nb(nb)
    {
        keysQueue = xQueueCreate(3, sizeof(KeyActions));
    }

    ~KeyBinder()
    {
        unbindKeys();
        vQueueDelete(keysQueue);
    }

    virtual void bindKeys()
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        nb->setKeyQueue(keysQueue);
        createKeyEventsTask();
    }
    virtual void unbindKeys()
    {
        nb->setKeyQueue(NULL);
    }
};
