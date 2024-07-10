#include <Arduino.h>
#include "main.h"

void setup()
{
  Serial.begin(115200);
  neo.begin(9600);
  pinMode(neo_PPS, INPUT);
  pinMode(button, INPUT);

  net.init();
  while (!net.begin())
  {
    Serial.println("Error connecting to network");
    delay(1000);
  }
  Body_Params body = {"id", net.getMacAddress()};
  while (uuid == "")
  {
    HTTP_Request req = net.POST(json, &body, 1, "/api/device/sign");
    if (req.code == 200)
    {
      uuid = req.body;
      Serial.println("Get uuid successfully");
    }
    else
      Serial.println("Error getting uuid");
    delay(1000);
  }
}

void loop()
{
  if (neo.available())
  {
    if (!neo_isDataStarted && neo.peek() == '$')
    {
      neo_isDataStarted = true;
      neo_bufferIndex = 0;
    }
    if (!neo_isDataStarted)
    {
      neo.read();
      return;
    }

    char c = neo.read();
    neo_buffer[neo_bufferIndex] = c;
    if (c == '\n') // if end of string
    {
      Data_Parse(neo_buffer, neo_bufferIndex);
      neo_isDataStarted = false;
      // printInfo();
    }
    neo_bufferIndex++;
  }

  unsigned long now = millis();
  if (now - lastTime > sendGPSDelay)
  {
    Serial.println(uuid);
    button_state = true;
    lastTime = now;
    Serial.println("Send GPS to server");
    GPS_GGA_t *gga = &neo_data.gga;
    if (gga->status == 0)
    {
      Serial.println("Invalid GGA GPS");
      return;
    }
    sendGPS(gga, "/api/data/updata");
  }

  if (digitalRead(button) == LOW && !button_state) // if button pressed
  {
    button_state = true;
    unsigned long now = millis();
    if (now - lastTime < 1000)
      return;
    lastTime = now;
    Serial.println("Send GPS to server");
    GPS_GGA_t *gga = &neo_data.gga;
    if (gga->status == 0)
    {
      Serial.println("Invalid GGA GPS");
      return;
    }
    sendGPS(gga);
  }
  else if (digitalRead(button) == HIGH && button_state)
    button_state = false;
}

void sendGPS(GPS_GGA_t *gga, String path)
{
  Body_Params body[] = {
      {"id", uuid},
      {"state", String(gga->status)},
      {"time", gga->time.toString()},
      {"lat", String(gga->position.lat, 6)},
      {"lat_dir", gga->position.lat_dir},
      {"lng", String(gga->position.lng, 6)},
      {"lng_dir", gga->position.lng_dir},
      {"satellites", String(gga->satellites)},
      {"hdop", String(gga->hdop)},
      {"altitude", String(gga->altitude)},
      {"geoid", String(gga->geoid)},
  };
  HTTP_Request req = net.POST(json, body, 11, path);
  if (req.code == 200)
    Serial.println("Send GPS to server successfully");
  else
    Serial.println("Error sending GPS to server");
}
