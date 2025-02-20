#include "orionAirQuality.h"
#include <Adafruit_PM25AQI.h>
#include <WiFi.h>
#include <NTPClient.h>

orionAirQuality::orionAirQuality(int id){
    // Constructor
    _id = id;
}

void orionAirQuality::init(WiFiClient *wifi, Adafruit_PM25AQI *aqi, NTPClient *timeClient, const char *fiware_server, const uint16_t *fiware_port, const char *fiware_device){
    // Initialize the library
    _client = wifi;
    _aqi = aqi;
    _timeClient = timeClient;
    _fiware_server = fiware_server;
    _fiware_port = fiware_port;
    _fiware_device = fiware_device;

    Serial.println("Air quality sensor initialized");
    Serial.println("ID: " + String(_id));
    Serial.println("Fiware server: " + String(_fiware_server));
    Serial.println("Fiware port: " + String(*_fiware_port));
    Serial.println("Fiware device: " + String(_fiware_device));
}

void orionAirQuality::update(){
    // Update the air quality measures
    if (! _aqi -> read(&data)){
        read_status = 0;
        return;
    }
    read_status = 1;
}

void orionAirQuality::upload(){
    // Upload the air quality measures to Orion Context Broker
    //update();
    if (_aqi -> read(&data)){
        if (WiFi.status() != WL_CONNECTED){
            Serial.println("Wifi not connected");
            return;
        }
        Serial.println("Uploading air quality measures to Orion Context Broker");
        postMeasurements();
        Serial.println("Upload success");
    }
    else{
        Serial.println("Could not read air quality measures");
    }   
}

void orionAirQuality::print(){
    // Print the air quality measures
    if (!read_status == 0){
        Serial.println("Could not read air quality measures");
        return;
    }
    Serial.println();
    Serial.println(F("---------------------------------------"));
    Serial.println(F("Concentration Units (standard)"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
    Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
    Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
    Serial.println(F("Concentration Units (environmental)"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
    Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
    Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
    Serial.println(F("---------------------------------------"));
    Serial.print(F("Particles > 0.3um / 0.1L air:")); Serial.println(data.particles_03um);
    Serial.print(F("Particles > 0.5um / 0.1L air:")); Serial.println(data.particles_05um);
    Serial.print(F("Particles > 1.0um / 0.1L air:")); Serial.println(data.particles_10um);
    Serial.print(F("Particles > 2.5um / 0.1L air:")); Serial.println(data.particles_25um);
    Serial.print(F("Particles > 5.0um / 0.1L air:")); Serial.println(data.particles_50um);
    Serial.print(F("Particles > 10 um / 0.1L air:")); Serial.println(data.particles_100um);
}

void orionAirQuality::postMeasurements(){
    // Post the air quality measures to Orion Context Broker

    String message;

    // change everything before so that instead of using single quotes, use double quotes, and instead of using double quotes, use double double quotes
    message += "{\"id\": \"urn:ngsi-ld:airQuality:" + String(_id) + ":" + String(_timeClient->getEpochTime()) + "\", ";
    message += "\"@context\" : [\"https://uri.etsi.org/ngsi-ld/v1/ngsi-ld-core-context.jsonld\"], ";
    message += "\"type\":\"airQuality\", ";
    message += "\"PM10\":{\"type\":\"Property\",\"value\":" + String(data.pm100_standard) + "}, ";
    message += "\"PM2.5\":{\"type\":\"Property\",\"value\":" + String(data.pm25_standard) + "}, ";
    message += "\"PM1.0\":{\"type\":\"Property\",\"value\":" + String(data.pm10_standard) + "}, ";
    message += "\"timestamp\":{\"type\":\"Property\",\"value\":" + String(_timeClient->getEpochTime()) + "}}";

    Serial.println("Message to send:");
    Serial.println(message);

    if (_client->connect(_fiware_server, *_fiware_port)){
        Serial.println("Connected to server");
        _client->println("POST /ngsi-ld/v1/entities/ HTTP/1.1");
        _client->println("Host: ec2-18-116-49-4.us-east-2.compute.amazonaws.com:1027");
        _client->println("User-Agent: " + String(_fiware_device));
        _client->println("Content-Type: application/ld+json");
        _client->println("Content-Length: " + String(message.length()));
        _client->println();
        _client->println(message);
        _client->println();
        Serial.println("Message sent");
    }
    else{
        Serial.println("Connection failed");
    }
}

/*
Serial.println("connected to server");
    client.println("POST /NGSI10/queryContext HTTP/1.1");
    client.println("Host: 130.206.82.115:1026");
    client.println("User-Agent: Arduino/1.1");
    client.println("Connection: close");
    client.println("Content-Type: application/xml");
    client.print("Content-Length: ");
    client.println("227");
    client.println();
    client.println("<?xml version=\"1.0\" encoding=\"UFT-8\"?>");
    client.println("<queryContextRequest>");
    client.println("<entityIdList>");
    client.println("<entityId type=\"UPCT:ACTUATOR\" isPattern=\"false\">");
    client.println("<id>UPCT:ACTUATOR:1</id>");
    client.println("</entityId>");
    client.println("</entityIdList>");
    client.println("<attributeList/>");
    client.println("</queryContextRequest>");
    client.println();

    Serial.println("XML Sent");*/