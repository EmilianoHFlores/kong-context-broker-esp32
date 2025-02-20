// Library for handling updates if air quality to Orion Context Broker
#ifndef orionAirQuality_h
#define orionAirQuality_h
#include <Arduino.h>
#include "Adafruit_PM25AQI.h"
#include <WiFi.h>
#include <NTPClient.h>

class orionAirQuality{
    public:
        orionAirQuality(int id);
        void init(WiFiClient *wifi, Adafruit_PM25AQI *aqi, NTPClient *timeClient, const char *_fiware_server, const uint16_t *_fiware_port, const char *_fiware_device);
        void update();
        void upload();
        void print();
        void postMeasurements();
    private:
        int _id;
        Adafruit_PM25AQI *_aqi;
        WiFiClient *_client;
        NTPClient *_timeClient;
        PM25_AQI_Data data;
        const char *_fiware_server;
        const uint16_t *_fiware_port;
        const char *_fiware_device;
        int read_status;
};


#endif