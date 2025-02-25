// Library for handling updates if air quality to Orion Context Broker
#ifndef orionAirQuality_h
#define orionAirQuality_h
#include <Arduino.h>
#include "Adafruit_PM25AQI.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <ArduinoJson.h>

class orionAirQuality{
    public:
        orionAirQuality(int id);
        void init(WiFiClient *wifi, Adafruit_PM25AQI *aqi, NTPClient *timeClient, const char *kong_url, const char *kong_token, const char *kong_ip, const uint16_t kong_port);
        void update();
        void upload();
        void print();
        void postMeasurements();
        String getToken();
    private:
        int _id;
        Adafruit_PM25AQI *_aqi;
        WiFiClient *_client;
        NTPClient *_timeClient;
        PM25_AQI_Data data;
        const char *_kong_url;
        const char *_kong_token;
        const char *_kong_ip;
        JsonDocument _json_handler;
        uint16_t _kong_port;
        int read_status;
};


#endif