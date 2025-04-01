
#include <Arduino.h>
#include <esp_sleep.h>  


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

//############### CONSTANTES ######################
#define uS_TO_MIN_FACTOR 60000000  // Conversión microsegundos a minutos
#define UPLOAD_PERIOD 10           // Minutos

//############### VARIABLES PERSISTENTES ##########
RTC_DATA_ATTR bool firstBoot = true;        // Solo inicializar hardware una vez
RTC_DATA_ATTR unsigned long bootCount = 0;  // Contador de reinicios

//################ OBJETOS ########################
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
SensirionI2cScd4x scd4x;
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

particleSensor pmSensor(1, &aqi, &timeClient);
scd41Sensor scdSensor(1, &scd4x, &timeClient);
kongContextBroker contextBroker(Constants::KONG_URL, Constants::KEYCLOAK_URL);

//############### FUNCIONES #######################
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

//################ SETUP ##########################
void setup() {
  Serial.begin(115200);
  

  if (firstBoot) {
    Wire.begin();
    while (!Serial) delay(10);
    scdSensor.init(0x62);
    Serial.println("Parque Central Air Quality Sensor");


    delay(1000);

    Serial2.begin(9600);
    
    if (!aqi.begin_UART(&Serial2)) {
      Serial.println("Could not find PM 2.5 sensor!");
      while (1) delay(10);
    }
    
    firstBoot = false;
    Serial.println("PM25 found!");
  }

  //---- Conexión WiFi y NTP ----//
  if (connectWifi(10)) {
    Serial.println("Wifi connected");
    timeClient.begin();
    timeClient.setTimeOffset(-21600);  // Zona horaria
    timeClient.update();
    Serial.println("Hora actual: " + timeClient.getFormattedTime());
  } else {
    Serial.println("Could not connect to wifi");
  }

  uploadParticleSensor();
  uploadScdSensor();

  WiFi.disconnect(true);  
  esp_sleep_enable_timer_wakeup(UPLOAD_PERIOD * uS_TO_MIN_FACTOR);
  

  Serial2.end();  // UART PM
  Wire.end();     // I2C SCD41
  
  Serial.println("Entrando en deep-sleep...");
  delay(200);  
  esp_deep_sleep_start();
}


void loop() {
}
