#ifndef __TYPE__H__
#define __TYPE__H__

#include <stdint.h>
#include <WString.h>

const String Satellite_str[] = {"GP", "GL", "GN"}; // GPS, GLONASS, GNSS

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
} GPS_GLL_t;                 // Geographic Position - Latitude/Longitude

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
} GPS_GGA_t;                 // Global Positioning System Fix Data

typedef struct
{
    GPS_GGA_t gga;
    GPS_GLL_t gll;
} GPS_Data_t;

#endif // __TYPE__H__