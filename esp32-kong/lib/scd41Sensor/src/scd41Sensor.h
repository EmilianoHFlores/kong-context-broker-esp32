// Library for handling updates if air quality to Orion Context Broker
#ifndef scd41Sensor_h
#define scd41Sensor_h
#include <Arduino.h>
#include "NTPClient.h"
#include <SensirionI2cScd4x.h>
#include "Wire.h"
#include "entity.h"

class scd41Sensor{
    public:
        scd41Sensor(int id, SensirionI2cScd4x *scd4x, NTPClient *timeClient);
        void init(int address);
        bool update();
        entity *createEntity();
    private:
        int _id;
        SensirionI2cScd4x *_scd4x;
        NTPClient *_timeClient;
        uint16_t _co2 = 0;
        float _temperature = 0.0f;
        float _humidity = 0.0f;
        bool _read_status = false;
};


#endif