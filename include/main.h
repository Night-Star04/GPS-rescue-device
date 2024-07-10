#ifndef __MAIN__H__
#define __MAIN__H__

#include <stdint.h>
#include <WString.h>

#include "pin.h"

// ===== NEO GPS =====
// === Library ===
#include <SoftwareSerial.h>
#include "type.h"
// === Variables ===
SoftwareSerial neo(neo_rx_pin, neo_tx_pin); // RX, TX
bool neo_isDataStarted = false;             // flag to indicate if data has started
GPS_Data_t neo_data;                        // data structure to store GPS data
char neo_buffer[128] = {""};                // buffer to store GPS data
uint8_t neo_bufferIndex = 0;                // index of buffer
// === Functions ===
bool Data_Verify(const char *str, uint8_t len);
bool Data_Checksum(const char *str, uint8_t len);
void Data_Parse(const char *str, uint8_t len);
void Position_Coversion(GPS_Position_t *position, String lat, String lat_dir, String lng, String lng_dir);
void Time_Coversion(Time_t *data, String time);
void printInfo();
#include "gps.cpp"

// ===== Network =====
// === Library ===
#include "network.h"
// === Config ===
static const char *network_ssid = "";                   // Network SSID
static const char *network_pwd = "";                    // Network password
static const char *network_host = "http://192.168.0.1"; // Network host
static const uint16_t network_port = 3000;              // Network port
static const unsigned long sendGPSDelay = 5000;         // delay between sending GPS data to server
// === Variables ===
Network net(network_ssid, network_pwd, network_host, network_port); // SSID, PASSWORD, HOST, PORT
String uuid = "";                                                   // device uuid
// === Functions ===
void sendGPS(GPS_GGA_t *gga, String path = "/api/data/log");

// ===== Button =====
// === Variables ===
bool button_state = false;  // button state
unsigned long lastTime = 0; // last time button pressed

#endif // __MAIN__H__