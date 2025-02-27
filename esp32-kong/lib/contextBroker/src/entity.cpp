#include "entity.h"
#include <NTPClient.h>

entity::entity(const char *type, const char *id, NTPClient *timeClient) {
    _type = type;
    _id = id;
    // _data += "{\"id\": \"urn:ngsi-ld:" + String(type) + ":" + String(id) + "\", ";
    _data += "{\"id\": \"urn:ngsi-ld:" + String(type) + ":" + String(id) + "-" + String(timeClient->getEpochTime()) + "\", ";
    _data += "\"type\":\"" + String(type) + "\", ";
    _data += "\"@context\" : [\"https://uri.etsi.org/ngsi-ld/v1/ngsi-ld-core-context-v1.6.jsonld\"], ";
}

void entity::addAttribute(const char *name, const char *value) {
    _data += "\"" + String(name) + "\":{\"type\":\"Property\",\"value\":" + String(value) + "}, ";
}

String entity::getEntityData() {
    // Remove the last comma and space before closing the JSON object
    String dataCopy = _data;
    dataCopy = dataCopy.substring(0, dataCopy.length() - 2);
    dataCopy += "}";
    return dataCopy;
}

const char* entity::getEntityId() {
    return _id;
}

const char* entity::getEntityType() {
    return _type;
}