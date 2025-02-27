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

void particleSensor::update(){
    // Update the air quality measures
    if (! _aqi -> read(&_data)){
        read_status = 0;
        return;
    }
    read_status = 1;
}

entity *particleSensor::createEntity(){
    // Create an entity with the air quality measures
    entity *sensorEntity = new entity("ParticleSensor", String(_id).c_str(), _timeClient);
    sensorEntity->addAttribute("PM10", String(_data.pm100_standard).c_str());
    sensorEntity->addAttribute("PM2.5", String(_data.pm25_standard).c_str());
    sensorEntity->addAttribute("PM1.0", String(_data.pm10_standard).c_str());
    sensorEntity->addAttribute("timestamp", String(_timeClient->getEpochTime()).c_str());
    return sensorEntity;
}