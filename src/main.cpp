#include <Arduino.h>
#include <SoftwareSerial.h>
#include "network.h"

typedef struct
{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t millisecond;
  String toString() const
  {
    return String(hour) + ":" + String(minute) + ":" + String(second) + "." + String(millisecond);
  }
} Time_t;
typedef struct
{
  float lat;      // Latitude
  String lat_dir; // N or S
  float lng;      // Longitude
  String lng_dir; // E or W
} GPS_Position_t;
typedef struct
{
  Time_t time;             // Date and Time (UTC) - hhmmss.ss
  GPS_Position_t position; // Position
  bool isDataValid;        // Data Valid
  String mode;             // Mode A - Autonomous, D - Differential, E - Estimated, N - Not Valid, S - Simulator
} GPS_GLL_t;               // Geographic Position - Latitude/Longitude
typedef struct
{
  Time_t time;             // Date and Time (UTC) - hhmmss.ss
  GPS_Position_t position; // Position
  uint8_t status;          // GPS Status, 0=Invalid, 1=2D/3D, 2=DGNSS, 4=Fixed RTK, 5=Float RTK, 6=Dead Reckoning
  uint8_t satellites;      // Number of satellites in use, 00 - 12
  float hdop;              // Horizontal Dilution of Precision
  float altitude;          // Altitude above mean sea level
  float geoid;             // Height of geoid (mean sea level) above WGS84 ellipsoid
  float dgps_age;          // Time in seconds since last DGPS update
  uint16_t dgps_station;   // DGPS station ID number
} GPS_GGA_t;               // Global Positioning System Fix Data
typedef struct
{
  GPS_GGA_t gga;
  GPS_GLL_t gll;
} GPS_Data_t;
const String Satellite_str[] = {"GP", "GL", "GN"}; // GPS, GLONASS, GNSS

SoftwareSerial neo(D1, D2);                    // RX, TX
Network net("SSID", "PASSWORD", "HOST", 3000); // SSID, PASSWORD, HOST, PORT
const uint8_t neo_PPS = D5;                    // PPS pin
const uint8_t button = D6;                     // button pin

bool neo_isDataStarted = false;    // flag to indicate if data has started
GPS_Data_t neo_data;               // data structure to store GPS data
char neo_buffer[128] = {""};       // buffer to store GPS data
uint8_t neo_bufferIndex = 0;       // index of buffer
bool button_state = false;         // button state
unsigned long lastTime = 0;        // last time button pressed
unsigned long sendGPSDelay = 5000; // delay between sending GPS data to server
String uuid = "";                  // device uuid

bool Data_Verify(const char *str, uint8_t len);
bool Data_Checksum(const char *str, uint8_t len);
void Data_Parse(const char *str, uint8_t len);
void Position_Coversion(GPS_Position_t *position, String lat, String lat_dir, String lng, String lng_dir);
void Time_Coversion(Time_t *data, String time);
void printInfo();
void sendGPS(GPS_GGA_t *gga, String path = "/api/data/log");

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

bool Data_Verify(const char *str, uint8_t len)
{
  if (str[0] != '$' || str[1] != 'G' || str[len - 1] != '\r' || str[len] != '\n') // if not a valid string
    return false;
  return true;
}

uint8_t hexCharToUint8(const char hexChar)
{
  if (hexChar >= '0' && hexChar <= '9')
    return hexChar - '0';
  else if (hexChar >= 'A' && hexChar <= 'F')
    return hexChar - 'A' + 10;
  else if (hexChar >= 'a' && hexChar <= 'f')
    return hexChar - 'a' + 10;
  else
    return 0; // Invalid character
}

uint8_t hexStrToUint8(const char *str, size_t len)
{
  return hexCharToUint8(str[len - 1]) * 16 + hexCharToUint8(str[len - 0]);
}

bool Data_Checksum(const char *str, uint8_t len)
{
  len -= 2; // remove '\r\n'
  uint8_t checksum = 0;
  for (uint8_t i = 1; i < len; i++) // skip the first character '$'
  {
    if (str[i] == '*') // if '*' character
      break;
    checksum ^= str[i];
  }
  uint8_t checksum_received = hexStrToUint8(str, len);
  if (checksum == checksum_received)
    return true;
  return false;
}

void Position_Coversion(GPS_Position_t *position, String lat, String lat_dir, String lng, String lng_dir)
{
  position->lat = lat.substring(0, 2).toFloat() + lat.substring(2).toFloat() / 60;
  position->lng = lng.substring(0, 3).toFloat() + lng.substring(3).toFloat() / 60;
  if (lat_dir == "S")
    position->lat *= -1;
  if (lng_dir == "W")
    position->lng *= -1;
  position->lat_dir = lat_dir; // N or S
  position->lng_dir = lng_dir; // E or W
}

void Time_Coversion(Time_t *data, String time)
{
  data->hour = time.substring(0, 2).toInt();
  data->minute = time.substring(2, 4).toInt();
  data->second = time.substring(4, 6).toInt();
  data->millisecond = time.substring(7).toInt();
}

void Data_Parse(const char *str, uint8_t len)
{
  if (!Data_Verify(str, len))
  {
    Serial.println("Invalid data");
    return;
  }
  else if (!Data_Checksum(str, len))
  {
    Serial.println("Invalid checksum");
    return;
  }

  int index = 0;                  // index of token
  String str_temp = "";           // temporary string
  static String token[25] = {""}; // array of string tokens

  for (uint8_t i = 1; i < len; i++) // skip the first character '$'
  {
    if (str[i] == ',' || str[i] == '*' || i == len - 1) // if ',' or '*' or end of string
    {
      token[index] = str_temp;
      str_temp = "";
      index++;
    }
    else
      str_temp += str[i];
  }

  // parse data
  String satellite = token[0].substring(0, 2);
  if (satellite != Satellite_str[0] && satellite != Satellite_str[1] && satellite != Satellite_str[2]) // if not GPS, GLONASS, GNSS
  {
    Serial.println("Invalid satellite");
    return;
  }

  // GNGGA, GNGLL, GNGSA, GLGSV, GPGSV, GNRMC, GNVTG for NEO-M8M
  if (token[0] == "GNGGA")
  {
    GPS_GGA_t *gga = &neo_data.gga;
    Time_Coversion(&gga->time, token[1]);
    Position_Coversion(&gga->position, token[2], token[3], token[4], token[5]);
    gga->status = token[6].toInt();
    gga->satellites = token[7].toInt();
    gga->hdop = token[8].toFloat();
    gga->altitude = token[9].toFloat();
    gga->geoid = token[11].toFloat();
    // M8M does not have dgps_age and dgps_station
  }
  else if (token[0] == "GNGLL")
  {
    GPS_GLL_t *gll = &neo_data.gll;
    Position_Coversion(&gll->position, token[1], token[2], token[3], token[4]);
    Time_Coversion(&gll->time, token[5]);
    gll->isDataValid = token[6] == "A" ? true : false; // A - Data Valid, V - Data Invalid
    gll->mode = token[7];
  }
}

void printInfo()
{
  Serial.println("--------------------");
  Serial.println("GGA");
  if (neo_data.gga.status == 0)
    Serial.println("Invalid GGA GPS");
  else
  {
    Serial.print("Time: ");
    Serial.print(neo_data.gga.time.hour);
    Serial.print(":");
    Serial.print(neo_data.gga.time.minute);
    Serial.print(":");
    Serial.print(neo_data.gga.time.second);
    Serial.print(".");
    Serial.println(neo_data.gga.time.millisecond);
    Serial.print("Position: ");
    Serial.print(neo_data.gga.position.lat, 6);
    Serial.print(neo_data.gga.position.lat_dir);
    Serial.print(",");
    Serial.print(neo_data.gga.position.lng, 6);
    Serial.println(neo_data.gga.position.lng_dir);
    Serial.print("Status: ");
    Serial.print(neo_data.gga.status);
    Serial.print(", Satellites in use: ");
    Serial.print(neo_data.gga.satellites);
    Serial.print(", HDOP: ");
    Serial.print(neo_data.gga.hdop);
    Serial.print(", Altitude: ");
    Serial.print(neo_data.gga.altitude);
    Serial.print(", Geoid: ");
    Serial.println(neo_data.gga.geoid);
  }
  Serial.println("--------------------");
  Serial.println("GLL");
  if (!neo_data.gll.isDataValid)
    Serial.println("Invalid GLL GPS");
  else
  {
    Serial.print("Time: ");
    Serial.print(neo_data.gll.time.hour);
    Serial.print(":");
    Serial.print(neo_data.gll.time.minute);
    Serial.print(":");
    Serial.print(neo_data.gll.time.second);
    Serial.print(".");
    Serial.println(neo_data.gll.time.millisecond);
    Serial.print("Position: ");
    Serial.print(neo_data.gll.position.lat, 6);
    Serial.print(neo_data.gll.position.lat_dir);
    Serial.print(",");
    Serial.print(neo_data.gll.position.lng, 6);
    Serial.println(neo_data.gll.position.lng_dir);
    Serial.print("Mode: ");
    Serial.println(neo_data.gll.mode);
  }
  Serial.println("--------------------\r\n");
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
