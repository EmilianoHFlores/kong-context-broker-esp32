#ifndef Constants_h
#define Constants_h

#include <Arduino.h>

//################ WIFI VARIABLES ################
namespace Constants {
    
    static constexpr char WIFI_SSID[] = "xxxx";     // replace with your SSID
    static constexpr char WIFI_PASSWORD[] = "xxxx";     // replace with your PASSWORD
    //################ FIWARE VARIABLES ################
    static constexpr char KONG_URL[] = "xxxx";
    static constexpr char KONG_IP[] = "xxxx";
    static constexpr uint16_t KONG_PORT = 0000;
    static constexpr char KONG_TOKEN[] = "xxx";
    static constexpr char KEYCLOAK_URL[] = "xxxx";
    static constexpr char KEYCLOAK_CLIENT_ID[] = "xxxx";
    static constexpr char KEYCLOAK_CLIENT_SECRET[] = "xxxx";
    static constexpr char KEYCLOAK_IP[] = "xxxx";
    static constexpr uint16_t KEYCLOAK_PORT = 0000;
};

#endif