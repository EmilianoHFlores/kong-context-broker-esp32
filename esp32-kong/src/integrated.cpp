#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <esp_sleep.h>  
#include "Adafruit_PM25AQI.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "Constants.h"
#include "entity.h"
#include "kongContextBroker.h"
#include "particleSensor.h"
#include "scd41Sensor.h"
#include <SensirionI2cScd4x.h>
#include <Wire.h>

#define uS_TO_MIN_FACTOR 60000000
#define UPLOAD_PERIOD 10
#define FIRMWARE_VERSION "1.0.0"

const char* ssid = "ParqueCentral_EXT";
const char* password = "CienciasCiudades2024";
const char* HOST = "192.168.1.223";
const uint16_t PORT = 8000;
const String firmware_path = "/firmware_update";
const String version_path = "/firmware_version";

RTC_DATA_ATTR bool firstBoot = true;
RTC_DATA_ATTR bool isPreWake = false;

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
SensirionI2cScd4x scd4x;
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

particleSensor pmSensor(1, &aqi, &timeClient);
scd41Sensor scdSensor(1, &scd4x, &timeClient);
kongContextBroker contextBroker(Constants::KONG_URL, Constants::KEYCLOAK_URL);

// ========= OTA ============
bool checkFirmwareUpdate() {
    HTTPClient http;
    String url = "http://" + String(HOST) + ":" + String(PORT) + version_path;
    http.begin(url);
    if (http.GET() == HTTP_CODE_OK) {
        JsonDocument doc;
        deserializeJson(doc, http.getString());
        String new_version = doc["version"];
        return (new_version != FIRMWARE_VERSION);
    }
    return false;
}

void performUpdate(Stream& updateSource, size_t updateSize) {
    if (Update.begin(updateSize)) {
        size_t written = Update.writeStream(updateSource);
        if (written == updateSize) {
            Serial.println("Firmware descargado completo");
        }
        if (Update.end()) {
            if (Update.isFinished()) {
                Serial.println("Actualización finalizada. Reiniciando...");
                ESP.restart();
            }
        } else {
            Serial.println("Error en actualización: " + String(Update.getError()));
        }
    } else {
        Serial.println("No hay espacio suficiente para OTA");
    }
}

void updateFirmware() {
    HTTPClient http;
    String url = "http://" + String(HOST) + ":" + String(PORT) + firmware_path;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        int contentLength = http.getSize();
        performUpdate(*http.getStreamPtr(), contentLength);
    } else {
        Serial.println("Error descargando firmware: " + String(httpCode));
    }
    http.end();
}

// ========= Conexión y Sensores =========
bool connectWifi(int tries = 10) {
    Serial.println("Conectando a: " + String(ssid));
    WiFi.begin(ssid, password);
    for (int i = 0; i < tries; i++) {
        if (WiFi.status() == WL_CONNECTED) break;
        Serial.print(".");
        delay(1000);
    }
    Serial.println();
    return WiFi.status() == WL_CONNECTED;
}

void uploadSensorData() {
    if (pmSensor.update()) {
        entity* sensorEntity = pmSensor.createEntity();
        contextBroker.uploadEntity(sensorEntity);
        delete sensorEntity;
    }
    if (scdSensor.update()) {
        entity* sensorEntity = scdSensor.createEntity();
        contextBroker.uploadEntity(sensorEntity);
        delete sensorEntity;
    }
}

// void activatePins() {
//     Serial.println("Activando pines...");
//     int safeGPIOs[] = {
//         4, 16, 17
//     };

//     for (int i = 0; i < sizeof(safeGPIOs)/sizeof(safeGPIOs[0]); i++) {
//         int pin = safeGPIOs[i];
//         pinMode(pin, OUTPUT);
//         digitalWrite(pin, HIGH);
//         Serial.printf("GPIO %d -> HIGH\n", pin);
//         delay(5);
//     }
// }

// void deactivatePins() {
//     Serial.println("Apagando pines seguros...");

//     int safeGPIOs[] = { 4, 16, 17 };

//     for (int i = 0; i < sizeof(safeGPIOs) / sizeof(safeGPIOs[0]); i++) {
//         int pin = safeGPIOs[i];
//         Serial.print("Probando GPIO ");
//         Serial.println(pin);
//         pinMode(pin, OUTPUT);
//         digitalWrite(pin, LOW);
//         Serial.printf("GPIO %d -> LOW OK\n", pin);
//         delay(10);
//     }

//     Serial.println("Fin de desactivación.");
// }



// ========= Setup principal =========
// void setup() {
//     Serial.begin(115200);
//     WiFi.mode(WIFI_STA);

//     // if (isPreWake) {
//     //     // Serial.println("Pre-Wake: Activando pines antes del próximo ciclo");
//     //     // activatePins();
//     //     delay(60000); // 1 minuto
//     //     isPreWake = false;

//     //     esp_sleep_enable_timer_wakeup(1 * uS_TO_MIN_FACTOR);
//     //     Serial.println("Entrando a etapa final de sueño por 1 min...");
//     //     delay(200);
//     //     esp_deep_sleep_start();
//     //     return;
//     // }

//     if (firstBoot) {
//         Wire.begin();
//         scdSensor.init(0x62);
//         Serial2.begin(9600);
//         if (!aqi.begin_UART(&Serial2)) {
//             Serial.println("Error sensor PM2.5!");
//             while (true) delay(10);
//         }
//         firstBoot = false;
//         Serial.println("Dispositivo inicializado");
//     }

//     if (connectWifi()) {
//         timeClient.begin();
//         timeClient.setTimeOffset(-21600);
//         timeClient.update();

//         uploadSensorData();

//         if (checkFirmwareUpdate()) {
//             Serial2.end();
//             Wire.end();
//             updateFirmware();
//         }
//     }

//     WiFi.disconnect(true);
//     Serial2.end();
//     Wire.end();

//     // Serial.println("Desactivando todos los pines...");
//     // deactivatePins();

//     Serial.printf("Heap libre antes de dormir: %u\n", ESP.getFreeHeap());
//     isPreWake = true;
//     esp_sleep_enable_timer_wakeup((UPLOAD_PERIOD - 1) * uS_TO_MIN_FACTOR);
//     Serial.println("Entrando a deep sleep (etapa larga)...");
//     delay(200);
//     esp_deep_sleep_start();
// }


void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    // Siempre reinicializa estos periféricos tras deep sleep
    Wire.begin();
    scdSensor.init(0x62);
    Serial2.begin(9600);
    if (!aqi.begin_UART(&Serial2)) {
        Serial.println("Error sensor PM2.5!");
        while (true) delay(10);
    }

    Serial.println("Dispositivo inicializado");

    if (connectWifi()) {
        timeClient.begin();
        timeClient.setTimeOffset(-21600);
        timeClient.update();

        uploadSensorData();

        if (checkFirmwareUpdate()) {
            Serial2.end();
            Wire.end();
            updateFirmware();
        }
    }

    WiFi.disconnect(true);
    Serial2.end();
    Wire.end();

    Serial.printf("Heap libre antes de dormir: %u\n", ESP.getFreeHeap());
    esp_sleep_enable_timer_wakeup((UPLOAD_PERIOD) * uS_TO_MIN_FACTOR);
    Serial.println("Entrando a deep sleep...");
    delay(200);
    esp_deep_sleep_start();
}





void loop() {}
