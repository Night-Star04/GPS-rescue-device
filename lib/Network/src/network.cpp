#if defined ESP32 || defined ESP8266
#include "network.h"
Network::Network(
    const char *ssid, const char *password,
    const char *host, uint16_t port) : _ssid(ssid), _password(password),
                                       _host(host), _port(port) {}
Network::~Network(void) {}

void Network::init(void)
{
    led = new LED(LED_BUILTIN);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setAutoReconnect(true);

    isInit = true;
}

bool Network::begin(uint8_t MaxConnectNumber)
{
    if (!isInit)
        return false;

    led->begin();
    WiFi.begin(_ssid, _password);
    uint8_t connection = 0;
#ifdef Network_DeBug
    Serial.print(F("Connecting to WiFi"));
#endif

    while (connection < MaxConnectNumber && !isConnect)
    {
        led->toggle();
        delay(100);
        led->toggle();
        delay(100);

        delay(1000);

        if (WiFi.status() == WL_CONNECTED)
            isConnect = true;

#ifdef Network_DeBug
        Serial.print(".");
#endif
        connection++;
    }
    if (!isConnect)
    {
#ifdef Network_DeBug
        Serial.print("\r\n");
        Serial.println(F("Failed to connect to WiFi"));
#endif
        return false;
    }

#ifdef Network_DeBug
    Serial.println(F("Connected to WiFi"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
    WiFi.printDiag(Serial);
#endif

    led->on();
    return true;
}

HTTP_Request Network::GET(HTTP_Params params[], uint8_t size, String path)
{
    if (!isConnect || !isInit || !run())
        return {String(""), String(""), 0, true};

    WiFiClient client;
    HTTPClient http;

    HTTP_Request req;
    String value;
    int httpResponseCode;
    String url = url_merge(params, size, path);

    http.begin(client, url.c_str());
    httpResponseCode = http.GET();

    return httpReq(&http, url, httpResponseCode);
}

HTTP_Request Network::GET(String arg, String data, String path)
{
    HTTP_Params params[1] = {
        {arg, data}};
    return GET(params, 1, path);
}

HTTP_Request Network::GET(String path)
{
    return GET(NULL, 0, path);
}

HTTP_Request Network::POST(HTTP_Params params[], uint8_t size, String path)
{
    return POST(params, size, none, "", path);
}

HTTP_Request Network::POST(Body_Type type, Body_Params body[], uint8_t body_size, String path)
{
    return POST(NULL, 0, type, body, body_size, path);
}

HTTP_Request Network::POST(HTTP_Params params[], uint8_t size, Body_Type type, String body, String path)
{
    if (!isConnect || !isInit || !run())
        return {String(""), String(""), 0, true};

    WiFiClient client;
    HTTPClient http;

    HTTP_Request req;
    String value;
    int httpResponseCode;
    String url = url_merge(params, size, path);

    http.begin(client, url.c_str());

    switch (type)
    {
    case x_www_form_urlencoded:
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        break;
    case json:
        http.addHeader("Content-Type", "application/json");
        break;
    case raw:
        http.addHeader("Content-Type", "text/plain");
        break;
    default:
        break;
    }

    httpResponseCode = http.POST(body);

    return httpReq(&http, url, httpResponseCode);
}

HTTP_Request Network::POST(HTTP_Params params[], uint8_t size, Body_Type type, Body_Params body[], uint8_t body_size, String path)
{
    String bodyData = "";

    switch (type)
    {
    case x_www_form_urlencoded:
        if (body != NULL)
        {
            for (uint8_t i = 0; i < body_size; i++)
                bodyData += (i != 0 ? "&" : "") + body[i].key + "=" + body[i].value;
        }
        break;
    case json:
        if (body != NULL)
        {
            bodyData += "{";
            for (uint8_t i = 0; i < body_size; i++)
            {
                bodyData += (i != 0 ? ",\"" : "\"") + body[i].key + "\":\"" + body[i].value + "\"";
            }
            bodyData += "}";
        }
        break;
    case raw:
        if (body != NULL)
        {
            for (uint8_t i = 0; i < body_size; i++)
                bodyData += body[i].value;
        }
        break;
    default:
        break;
    }

    return POST(params, size, type, bodyData, path);
}

HTTP_Request Network::POST(String path)
{
    return POST(NULL, 0, none, "", path);
}

String Network::url_merge(HTTP_Params params[], uint8_t size, String path)
{
    String url = String(_host) + ":" + String(_port) + path;

    if (params != NULL)
    {
        String postData = "?";
        for (uint8_t i = 0; i < size; i++)
            postData += (i != 0 ? "&" : "") + params[i].arg + "=" + params[i].data;
        url += postData;
    }

#ifdef Network_DeBug
    Serial.printf("URL: %s\n", url.c_str());
#endif

    return url;
}

HTTP_Request Network::httpReq(HTTPClient *http, String url, int code)
{
    HTTP_Request req;
    String value;

    req.code = code;
    req.url = url;

    if (code > 0)
    {
        value = http->getString();

#ifdef Network_DeBug
        Serial.print(F("HTTP Response code: "));
        Serial.printf("%d\nString: %s\n", code, value.c_str());
#endif

        req.isError = false;

        led->on();
    }
    else
    {
        value = http->errorToString(code);

#ifdef Network_DeBug
        Serial.printf("Error code: %d String: %s\n", code, value.c_str());
#endif

        req.isError = true;

        led->off();
        led->toggle();
        delay(100);
        led->toggle();
    }

    http->end();

    req.body = value;

    return req;
}

bool Network::run(void)
{
    if (!isInit)
        return false;

    if (WiFi.status() == WL_CONNECTED)
    {
        isConnect = true;
        led->on();
        return true;
    }

#ifdef Network_DeBug
    Serial.print(F("Failed to connect to WiFi"));
#endif

    isConnect = false;
    led->off();
    return false;
}

String Network::getMacAddress(void)
{
    return WiFi.macAddress();
}

#endif
