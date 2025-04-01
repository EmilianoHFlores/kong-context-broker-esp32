#ifndef Constants_h
#define Constants_h

#include <Arduino.h>

//################ WIFI VARIABLES ################
namespace Constants {
    
    static constexpr char WIFI_SSID[] = "ParqueCentral_EXT";     // replace with your SSID
    static constexpr char WIFI_PASSWORD[] = "CienciasCiudades2024";     // replace with your PASSWORD
    //################ FIWARE VARIABLES ################
    static constexpr char KONG_URL[] = "http://40.84.231.179:8000/orion/ngsi-ld/v1/entities";
    static constexpr char KONG_IP[] = "40.84.231.179";
    static constexpr uint16_t KONG_PORT = 8000;
    static constexpr char KONG_TOKEN[] = "eyJhbGciOiJSUzI1NiIsInR5cCIgOiAiSldUIiwia2lkIiA6ICJLQ1RxeFl5T2VxXzRMeHk3NktVVDdhQVB6aFcwUWNoSk5ERHJ6TnFHRVJrIn0.eyJleHAiOjE3NDAwMzg3ODAsImlhdCI6MTc0MDAzODcyMCwianRpIjoiZDM2YjcwNzYtM2Y1Ni00MGY4LThlNDctMzQxMjQyYWY5NTdjIiwiaXNzIjoiaHR0cDovLzQwLjg0LjIzMS4xNzk6ODA4MC9yZWFsbXMvbWFzdGVyIiwiYXVkIjpbIm1hc3Rlci1yZWFsbSIsImFjY291bnQiXSwic3ViIjoiODIxZTA0ZmMtNDY0Mi00NWY5LWE3MTItMGM5ZjQ1MjhiMTdkIiwidHlwIjoiQmVhcmVyIiwiYXpwIjoia29uZ19jbGllbnQiLCJhY3IiOiIxIiwiYWxsb3dlZC1vcmlnaW5zIjpbImh0dHA6Ly8qIl0sInJlYWxtX2FjY2VzcyI6eyJyb2xlcyI6WyJkZWZhdWx0LXJvbGVzLW1hc3RlciIsIm9mZmxpbmVfYWNjZXNzIiwidW1hX2F1dGhvcml6YXRpb24iXX0sInJlc291cmNlX2FjY2VzcyI6eyJtYXN0ZXItcmVhbG0iOnsicm9sZXMiOlsiY3JlYXRlLWNsaWVudCJdfSwiYWNjb3VudCI6eyJyb2xlcyI6WyJtYW5hZ2UtYWNjb3VudCIsIm1hbmFnZS1hY2NvdW50LWxpbmtzIiwidmlldy1wcm9maWxlIl19fSwic2NvcGUiOiJlbWFpbCBwcm9maWxlIiwiY2xpZW50SG9zdCI6IjE4OS4yMTguMi42MiIsImVtYWlsX3ZlcmlmaWVkIjpmYWxzZSwicHJlZmVycmVkX3VzZXJuYW1lIjoic2VydmljZS1hY2NvdW50LWtvbmdfY2xpZW50IiwiY2xpZW50QWRkcmVzcyI6IjE4OS4yMTguMi42MiIsImNsaWVudF9pZCI6ImtvbmdfY2xpZW50In0.btG4e8uruUFhFgpVPynf2Jjj1ZLkZdJwPT0m2RkUsdLY8n_9foUSQq1-c2D2uHfxUjBCVWp8iu5VfhR0lcFT-9Q1MLjLDrVUKKZKoS3GIh4s-MVDXvWV_hf6j3cEchnmBstKeH8VRFNhZNPBVIf4Xtuk55iIK7hxX9TT2ilyVBZHJEAUN2Nnkas7NoRi0G1xMT4xO0km8b23-mByrQNUs_zuIpRMp3zw_g05Ws-MKERd4CaYwstBGRD6VmJ_OuY25bAzspbBk8NIxKmyuOuV3XSSvQR2bCcD_q6BS7oEX6D-ZENN2G5rU2U_mk4RNxDO3Y7-F_klpPtI18rUyUXrNw";
    static constexpr char KEYCLOAK_URL[] = "http://40.84.231.179:8080/realms/master/protocol/openid-connect/token";
    static constexpr char KEYCLOAK_CLIENT_ID[] = "kong_client";
    static constexpr char KEYCLOAK_CLIENT_SECRET[] = "FOihFkvVFhMkRzMNpEMoaO6ByuPI5fLa";
    static constexpr char KEYCLOAK_IP[] = "40.84.231.179";
    static constexpr uint16_t KEYCLOAK_PORT = 8080;
};

#endif