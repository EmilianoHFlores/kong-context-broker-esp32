// Library for handling updates if air quality to Orion Context Broker
#ifndef entity_h
#define entity_h
#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>

class entity{
    public:
        entity(const char *type, const char *id, NTPClient *timeClient);
        void addAttribute(const char *name, const char *value);
        String getEntityData();
        const char *getEntityId();
        const char *getEntityType();
    private:
        const char *_type;
        const char *_id;
        String _data;
};


#endif