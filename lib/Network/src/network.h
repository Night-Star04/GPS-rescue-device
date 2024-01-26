#if defined ESP32 || defined ESP8266
#ifndef __NETWORK_H__
#define __NETWORK_H__
#include <Arduino.h>
#include "LED.h"
#include "./type.h"

#define Network_DeBug

#ifdef ESP32
#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#endif

class Network
{
public:
    Network(const char *ssid, const char *password, const char *host, uint16_t port = 80);
    ~Network();

    void init();
    bool begin(uint8_t MaxConnectNumber = 10);

    HTTP_Request GET(HTTP_Params params[], uint8_t size, String path = "/");
    HTTP_Request GET(String arg, String data, String path = "/");
    HTTP_Request GET(String path = "/");

    HTTP_Request POST(HTTP_Params params[], uint8_t size, String path = "/");
    HTTP_Request POST(Body_Type type, Body_Params body[], uint8_t body_size, String path = "/");
    HTTP_Request POST(HTTP_Params params[], uint8_t size, Body_Type type, String body, String path = "/");
    HTTP_Request POST(HTTP_Params params[], uint8_t size, Body_Type type, Body_Params body[], uint8_t body_size, String path = "/");
    HTTP_Request POST(String path = "/");

    bool run(void);

    String getMacAddress(void);

protected:
    LED *led;

    const char *_ssid;
    const char *_password;
    const char *_host;
    uint16_t _port;

    bool isConnect = false;
    bool isInit = false;

    String url_merge(HTTP_Params params[], uint8_t size, String path);
    HTTP_Request httpReq(HTTPClient *http, String url, int code);
};
#endif // __NETWORK_H__

#else

#error "This library only supports boards with ESP32 or ESP8266 processor."

#endif
