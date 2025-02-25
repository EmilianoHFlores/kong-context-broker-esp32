#include "orionAirQuality.h"
#include <Adafruit_PM25AQI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <Arduino_JSON.h>
#include <Constants.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


orionAirQuality::orionAirQuality(int id){
    // Constructor
    _id = id;
}

void orionAirQuality::init(WiFiClient *wifi, Adafruit_PM25AQI *aqi, NTPClient *timeClient, const char *kong_url, const char *kong_token, const char *kong_ip, const uint16_t kong_port){
    // Initialize the library
    _client = wifi;
    _aqi = aqi;
    _timeClient = timeClient;
    _kong_url = kong_url;
    _kong_token = kong_token;
    _kong_ip = kong_ip;
    _kong_port = kong_port;

    Serial.println("Air quality sensor initialized");
    Serial.println("ID: " + String(_id));
    Serial.println("Kong URL: " + String(_kong_url));
    Serial.println("Kong Token: " + String(_kong_token));
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

// /*POST http://40.84.231.179:8080/realms/master/protocol/openid-connect/token
// User-Agent: python-requests/2.32.3
// Accept-Encoding: gzip, deflate
// Accept: */*
// Connection: keep-alive
// Content-Length: 98
// Content-Type: application/x-www-form-urlencoded
// */
String orionAirQuality::getToken(){
    // if (_client->connect("40.84.231.179", 8080)){
    //     Serial.println("Connected to server");
    //     _client->println("POST http://40.84.231.179:8080/realms/master/protocol/openid-connect/token");
    //     _client->println("User-Agent: python-requests/2.32.3");
    //     _client->println("Accept-Encoding: gzip, deflate");
    //     _client->println("Accept: */*");
    //     _client->println("Connection: keep-alive");
    //     _client->println("Content-Length: 98");
    //     _client->println("Content-Type: application/x-www-form-urlencoded");
    //     _client->println();
    //     _client->println("grant_type=client_credentials&client_id=kong_client&client_secret=FOihFkvVFhMkRzMNpEMoaO6ByuPI5fLa");
    //     _client->println();
    //     Serial.println("Message sent");
    // }
    if (!_client->connect(Constants::KEYCLOAK_IP, Constants::KEYCLOAK_PORT)){
        Serial.println("Connection failed");
    }
    else {
        Serial.println("Connection successful");
    }
    HTTPClient http;
    http.begin(Constants::KEYCLOAK_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // String query_string = "grant_type=client_credentials&client_id=kong_client&client_secret=FOihFkvVFhMkRzMNpEMoaO6ByuPI5fLa";
    String query_string = "grant_type=client_credentials&client_id=" + String(Constants::KEYCLOAK_CLIENT_ID) + "&client_secret=" + String(Constants::KEYCLOAK_CLIENT_SECRET);
    int httpCode = http.POST(query_string);
    String token;
    if (httpCode > 0){
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK){
            String payload = http.getString();
            Serial.println(payload);
            deserializeJson(_json_handler, payload);
            String token = _json_handler[String("access_token")];
            Serial.println("Token: "+ String(token));
            return token;
        }
    }
    else{
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        return token;
    }
    // // receive and store response
    // String response;
    // int timeout = 1000;
    // int i = 0;
    // while(!_client->available()){
    //     i++;
    //     if (i > timeout){
    //         Serial.println("Timeout");
    //         break;
    //     }
    //     delay(1);
    // }
    // if (_client->available()){
    //     response = _client->readString();
    //     Serial.println("Response:");
    //     Serial.println(response);
    // }
    // else{
    //     Serial.println("No response");
    // }
    // // parse response
    // const char* responseStr = response.c_str();
    // JSONVar parsedResponse = JSONVar::parse(responseStr);
    // String token = (const char*) parsedResponse["access_token"];
    // Serial.print("Token: ");
    // Serial.println(token);
}

void orionAirQuality::postMeasurements(){
    // Post the air quality measures to Orion Context Broker

    String message;

    message += "{\"id\": \"urn:ngsi-ld:ParticleSensor:" + String(_id) + "-" + String(_timeClient->getEpochTime()) + "\", ";
    message += "\"type\":\"ParticleSensor\", ";
    message += "\"@context\" : [\"https://uri.etsi.org/ngsi-ld/v1/ngsi-ld-core-context-v1.6.jsonld\"], ";
    message += "\"PM10\":{\"type\":\"Property\",\"value\":" + String(data.pm100_standard) + "}, ";
    message += "\"PM2.5\":{\"type\":\"Property\",\"value\":" + String(data.pm25_standard) + "}, ";
    message += "\"PM1.0\":{\"type\":\"Property\",\"value\":" + String(data.pm10_standard) + "}, ";
    message += "\"timestamp\":{\"type\":\"Property\",\"value\":" + String(_timeClient->getEpochTime()) + "}}";

    String token = getToken();
    Serial.println("Message to send:");
    Serial.println(message);

    if (!_client->connect(_kong_ip, _kong_port)){
        Serial.println("Connection failed");
    }
    else{
        Serial.println("Connected to server");
        HTTPClient http;
        http.begin(_kong_url);
        http.addHeader("Authorization", "Bearer " + token);
        http.addHeader("Content-Type", "application/ld+json");
        int httpCode = http.POST(message);
        if (httpCode > 0){
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
            if (httpCode == HTTP_CODE_OK){
                String payload = http.getString();
                Serial.println(payload);
            }
        }
        else{
            Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
    }

    // if (_client->connect(_kong_ip, _kong_port)){
    //     Serial.println("Connected to server");
    //     _client->println("POST " + String(_kong_url));
    //     _client->println("Authorization: Bearer " + String(token));
    //     _client->println("Content-Type: application/ld+json");
    //     _client->println("Content-Length: " + String(message.length()));
    //     _client->println();
    //     _client->println(message);
    //     _client->println();
    //     Serial.println("Message sent");
    // }
    // else{
    //     Serial.println("Connection failed");
    // }
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