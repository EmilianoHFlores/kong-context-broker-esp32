#include "particleSensor.h"
#include <Adafruit_PM25AQI.h>
#include <NTPClient.h>
#include "entity.h"

particleSensor::particleSensor(int id, Adafruit_PM25AQI *aqi, NTPClient *timeClient){
    // Initialize the library
    _id = id;
    _aqi = aqi;
    _timeClient = timeClient;
}

void particleSensor::init(){
    
}

bool particleSensor::update(){
    // Update the air quality measures
    if (! _aqi -> read(&_data)){
        return false;
    }
    return true;
}

entity *particleSensor::createEntity(){
    // Create an entity with the air quality measures
    entity *sensorEntity = new entity("ParticleSensor", String(_id).c_str(), _timeClient);
    sensorEntity->addAttribute("PM10", String(_data.pm100_standard).c_str());
    sensorEntity->addAttribute("PM2.5", String(_data.pm25_standard).c_str());
    sensorEntity->addAttribute("PM1.0", String(_data.pm10_standard).c_str());
    time_t rawTime = _timeClient->getEpochTime();
    struct tm *timeInfo = gmtime(&rawTime);
    char isoTime[30];
    snprintf(isoTime, sizeof(isoTime), "%04d-%02d-%02dT%02d:%02d:%02d.000000",
             timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
             timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    sensorEntity->addAttribute("dateObserved", String("\"" + String(isoTime) + "\"").c_str());
    return sensorEntity;
}