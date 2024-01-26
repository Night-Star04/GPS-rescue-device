#ifndef __NETWORK_TYPE_H__
#define __NETWORK_TYPE_H__

#include <Arduino.h>

typedef struct
{
    String arg;
    String data;
} HTTP_Params;

typedef struct
{
    String body;
    String url;
    int code;
    bool isError;
} HTTP_Request;

enum Body_Type
{
    none,
    x_www_form_urlencoded,
    json,
    raw
};

typedef struct
{
    String key;
    String value;
} Body_Params;

#endif
