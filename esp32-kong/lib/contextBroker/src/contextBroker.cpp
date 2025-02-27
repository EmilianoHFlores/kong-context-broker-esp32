#include "contextBroker.h"
#include "entity.h"
#include <HTTPClient.h>

contextBroker::contextBroker(const char *url){
    _url = url;
}

void contextBroker::uploadEntity(entity *entity){
    // Post the air quality measures to Orion Context Broker
    String message = entity->getEntityData();
    Serial.println("Message to send:");
    Serial.println(message);
}