#include "main.h"

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