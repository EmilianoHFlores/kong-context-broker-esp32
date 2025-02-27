#include "kongContextBroker.h"
#include "entity.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Constants.h>

kongContextBroker::kongContextBroker(const char *url, const char *keycloak_url) : contextBroker(url) {
    _url = url;
    _keycloak_url = keycloak_url;
}

void kongContextBroker::uploadEntity(entity *entity){
    // Post the air quality measures to Orion Context Broker
    updateToken();
    Serial.println("Sending entity");
    Serial.println(_url);
    Serial.println(_token);
    _http.begin(_url);

    _http.addHeader("Authorization", "Bearer " + _token);
    _http.addHeader("Content-Type", "application/ld+json");
    Serial.println(entity->getEntityData());
    int httpCode = _http.POST(entity->getEntityData());
    if (httpCode > 0){
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK){
            String payload = _http.getString();
            Serial.println(payload);
        }
    }
    else{
        Serial.printf("[HTTP] POST... failed, error: %s\n", _http.errorToString(httpCode).c_str());
    }
}

void kongContextBroker::updateToken() {
    // Get the token from Kong
    _keycloak_http.begin(_keycloak_url);
    _keycloak_http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // String query_string = "grant_type=client_credentials&client_id=kong_client&client_secret=FOihFkvVFhMkRzMNpEMoaO6ByuPI5fLa";
    String query_string = "grant_type=client_credentials&client_id=" + String(Constants::KEYCLOAK_CLIENT_ID) + "&client_secret=" + String(Constants::KEYCLOAK_CLIENT_SECRET);
    int httpCode = _keycloak_http.POST(query_string);

    if (httpCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = _keycloak_http.getString();
            deserializeJson(_json_handler, payload);
            _token = _json_handler[String("access_token")].as<String>();
        }
    }
    else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", _keycloak_http.errorToString(httpCode).c_str());
        _token = "";
    }
}
