// Library for handling updates if air quality to Orion Context Broker
#ifndef kongContextBroker_h
#define kongContextBroker_h
#include <Arduino.h>
#include "entity.h"
#include "contextBroker.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// INHerit from contextBroker
class kongContextBroker : public contextBroker{
    public:
        kongContextBroker(const char *url, const char *keycloak_url);
        void uploadEntity(entity *entity);
    private:
        const char *_url;
        const char *_keycloak_url;
        HTTPClient _http;
        HTTPClient _keycloak_http;
        String _token;
        JsonDocument _json_handler;
        void updateToken();
};

#endif