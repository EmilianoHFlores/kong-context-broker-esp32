#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <web_update.h>
#include <ArduinoJson.h>

String HOST = "192.168.1.223";
uint16_t PORT = 8000;
String firmware_dir = "/firmware_update";
String version_dir = "/firmware_version";
String curr_version = "1.0.0";

const char *ssid = "Roborregos";

const char *password = "RoBorregos2025";


void updateFirmware(String host, uint16_t port, String dir) {
    web_update webUpdate;
    // host String to char array
    char host_c[host.length() + 1];
    host.toCharArray(host_c, host.length() + 1);
    // dir String to char array
    char dir_c[dir.length() + 1];
    dir.toCharArray(dir_c, dir.length() + 1);
    webUpdate.host(host_c);
    webUpdate.hostPort(port);
    webUpdate.directory(dir_c);
    Serial.println("Updating firmware...");
    webUpdate.update_wifi();
    Serial.println("Firmware updated");
}

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "http://" + HOST + ":" + String(PORT) + version_dir;
        http.begin(url);
        int httpCode = http.GET();
        
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            JsonDocument doc;
            
            DeserializationError error = deserializeJson(doc, payload);
            if (!error) {
                String version = doc["version"];
                Serial.printf("Current firmware version: %s\n", version);
                if (version != curr_version) {
                    Serial.println("New firmware available");
                    updateFirmware(HOST, PORT, firmware_dir);
                    curr_version = version;
                } else {
                    Serial.println("Firmware is up to date");
                }
            }
        } else {
            Serial.printf("HTTP error: %d\n", httpCode);
            Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end();
    }
    Serial.println("On loop");
    Serial.println("AFTER UPDATE");
    delay(1000);  // Wait for 5 seconds
}
