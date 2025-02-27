#include "scd41Sensor.h"
#include <SensirionI2cScd4x.h>
#include <NTPClient.h>
#include "Wire.h"
#include "entity.h"

scd41Sensor::scd41Sensor(int id, SensirionI2cScd4x *scd4x, NTPClient *timeClient){
    // Initialize the library
    _id = id;
    _scd4x = scd4x;
    _timeClient = timeClient;
}

void scd41Sensor::init(int address){
    // Initialize the sensor
    _scd4x->begin(Wire, address); // Se agrega la dirección I2C
    _scd4x->startPeriodicMeasurement();
    
}

bool scd41Sensor::update(){
    // Update the air quality measures
    _scd4x->getDataReadyStatus(_read_status); // Corrección aquí
    
    if (_read_status) {
        _scd4x->readMeasurement(_co2, _temperature, _humidity);
        Serial.print("CO2: ");
        Serial.print(_co2);
        Serial.print(" ppm, Temperature: ");
        Serial.print(_temperature);
        Serial.print(" °C, Humidity: ");
        Serial.print(_humidity);
        Serial.println(" %");
        return true;
    } 
    Serial.println("Sensor data unavailable");
    return false;
}

entity *scd41Sensor::createEntity(){
    // Create an entity with the air quality measures
    entity *sensorEntity = new entity("EnvironmentSensor", String(_id).c_str(), _timeClient);
    sensorEntity->addAttribute("CO2", String(_co2).c_str());
    sensorEntity->addAttribute("Temperature", String(_temperature).c_str());
    sensorEntity->addAttribute("Humidity", String(_humidity).c_str());
    time_t rawTime = _timeClient->getEpochTime();
    struct tm *timeInfo = gmtime(&rawTime);
    char isoTime[30];
    snprintf(isoTime, sizeof(isoTime), "%04d-%02d-%02dT%02d:%02d:%02d.000000",
             timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
             timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    sensorEntity->addAttribute("dateObserved", String("\"" + String(isoTime) + "\"").c_str());
    return sensorEntity;
}