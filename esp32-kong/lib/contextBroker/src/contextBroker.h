// Library for handling updates if air quality to Orion Context Broker
#ifndef contextBroker_h
#define contextBroker_h
#include <Arduino.h>
#include "entity.h"
#include <HTTPClient.h>

class contextBroker{
    public:
        contextBroker(const char *_url);
        void uploadEntity(entity *entity);
    private:
        const char *_url;
        HTTPClient _http;

};


#endif