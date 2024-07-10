#ifndef __PIN__H__
#define __PIN__H__

#include <stdint.h>
#include <pins_arduino.h>

// ===== NEO GPS =====
const uint8_t neo_rx_pin = D1; // RX
const uint8_t neo_tx_pin = D2; // TX
const uint8_t neo_PPS = D5;    // PPS pin

// ===== Button =====
const uint8_t button = D6; // button pin

#endif // __PIN__H__