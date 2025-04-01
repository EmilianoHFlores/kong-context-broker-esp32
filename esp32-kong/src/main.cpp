#include <Arduino.h>

//######## 
#include "Adafruit_PM25AQI.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "Constants.h"
#include "entity.h"
#include "kongContextBroker.h"
#include "particleSensor.h"
#include "scd41Sensor.h"
#include <SensirionI2cScd4x.h>


#define SECONDS_PER_DAY 86400
#define UPLOAD_AT_EXACT_TIME 1

//################ FIWARE VARIABLES ################

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
SensirionI2cScd4x scd4x;
WiFiClient client;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
unsigned long curr_time = 0;
unsigned long last_time_update = 0;

//############### How much time between uploads (in minutes)######################
const uint32_t UPLOAD_PERIOD = 10; // minutes
bool allow_upload = false;
unsigned long last_upload_time = 0;
uint elapsed_time = 0;

bool connectWifi(int tries){
  Serial.println("Connecting to wifi: " + String(Constants::WIFI_SSID));
  int status = WL_IDLE_STATUS;
  WiFi.begin(Constants::WIFI_SSID, Constants::WIFI_PASSWORD);
  for (int i = 0; i < tries; i++){
      Serial.print("...");
      delay(10000);
      if (WiFi.status() == WL_CONNECTED){
          return true;
      }
  }
  return false;
}

bool checkPeriod(){

// NEED TO find a way to update time allowing for over >60 minutes, and also allowing for time to be updated via wifi 
// current idea is using elapsed time, just have to find a way to also update via wifi

  if (WiFi.status() == WL_CONNECTED && (timeClient.getHours()==0) || !allow_upload){
      timeClient.update();
      Serial.println("Time updated via wifi. H: " + String(timeClient.getHours()) + " M: " + String(timeClient.getMinutes()) + " S: " + String(timeClient.getSeconds()));
      last_time_update = timeClient.getEpochTime();
      // calculate an appropiate elapsed_time if upload has to be done at exact Hour
      if (UPLOAD_AT_EXACT_TIME){
          int current_cycle_minute = timeClient.getMinutes() % UPLOAD_PERIOD;
          elapsed_time = (current_cycle_minute)*60 + timeClient.getSeconds();
          Serial.print("Upload in ");
          Serial.print(UPLOAD_PERIOD - elapsed_time/60);
          Serial.println(" minutes");
          //special case, boot on exact hour needed
          if (current_cycle_minute == 0){
              allow_upload = true;
              return true;
          }
      }
      allow_upload = true;
  } else {
      elapsed_time += (timeClient.getEpochTime() - last_time_update);
      last_time_update = timeClient.getEpochTime();
  }

  if (allow_upload){
      if (elapsed_time > UPLOAD_PERIOD*60){
          elapsed_time = 0;
          return true;
      }
  }

  return false;
}

particleSensor pmSensor(1, &aqi, &timeClient);
scd41Sensor scdSensor(1, &scd4x, &timeClient);
kongContextBroker contextBroker(Constants::KONG_URL, Constants::KEYCLOAK_URL);

void setup() {
    // Wait for serial monitor to open
    Serial.begin(115200);
    Wire.begin();
    while (!Serial) delay(10);
    scdSensor.init(0x62);

    Serial.println("Parque Central Air Quality Sensor");

    // Wait one second for sensor to boot up!
    delay(1000);

    // If using serial, initialize it and set baudrate before starting!
    // Uncomment one of the following
    Serial2.begin(9600);
    //pmSerial.begin(9600);

    // There are 3 options for connectivity!
    //if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
    if (! aqi.begin_UART(&Serial2)) { // connect to the sensor over hardware serial
        //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
        Serial.println("Could not find PM 2.5 sensor!");
        while (1) delay(10);
    }

    Serial.println("PM25 found!");

    // Attempt to connect to wifi
    if (connectWifi(10)){
        Serial.println("Wifi connected");
    }
    else{
        Serial.println("Could not connect to wifi");
    }

    ArduinoOTA.setHostname("ESP32-OTA");  // Nombre del ESP32 en OTA
    ArduinoOTA.setPassword("CienciasCiudades2024");     // (Opcional) Contraseña OTA
    ArduinoOTA.begin();
    Serial.println("OTA Listo!");


    // Initialize a NTPClient to get time
    timeClient.begin();
    // Set offset time in seconds to adjust for your timezone, for example:
    // MEXICO GMT -6 = 3600 * -6 = -21600
    timeClient.setTimeOffset(-21600);

}

unsigned long test_time = 0;

void uploadParticleSensor(){
    if (pmSensor.update()){
        Serial.println("Read particle sensor successfully");
        entity *sensorEntity = pmSensor.createEntity();
        contextBroker.uploadEntity(sensorEntity);
    }
    else{
        Serial.println("Could not read particle sensor");
    }
}

void uploadScdSensor(){
    if (scdSensor.update()){
        Serial.println("Read environment sensor successfully");
        entity *sensorEntity = scdSensor.createEntity();
        contextBroker.uploadEntity(sensorEntity);
    }
    else{
        Serial.println("Could not read environment sensor");
    }
}

void loop() {
    ArduinoOTA.handle();
    if (checkPeriod()){
        Serial.println("Uploading...");
        uploadParticleSensor();
        uploadScdSensor();
    } else{
        if (WiFi.status()!= WL_CONNECTED){
            Serial.println("Reconnecting to wifi");
            WiFi.disconnect();
            WiFi.reconnect();
        }
    }
    // every second print out current time
    if (millis() - test_time > 1000){
        Serial.print("Local Hour:");
        Serial.print(timeClient.getHours());
        Serial.print(" Min:");
        Serial.print(timeClient.getMinutes());
        Serial.print(" Sec:");
        Serial.print(timeClient.getSeconds());
        Serial.print(" Elapsed time: ");
        Serial.println(elapsed_time);
        test_time = millis();

    }
}
