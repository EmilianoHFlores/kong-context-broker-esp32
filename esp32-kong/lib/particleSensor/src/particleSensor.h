// Library for handling updates if air quality to Orion Context Broker
#ifndef particleSensor_h
#define particleSensor_h
#include <Arduino.h>
#include "NTPClient.h"
#include "Adafruit_PM25AQI.h"
#include "entity.h"

class particleSensor{
    public:
        particleSensor(int id, Adafruit_PM25AQI *aqi, NTPClient *timeClient);
        void init();
        bool update();
        entity *createEntity();
    private:
        int _id;
        Adafruit_PM25AQI *_aqi;
        PM25_AQI_Data _data;
        NTPClient *_timeClient;
        int read_status;
};


#endif