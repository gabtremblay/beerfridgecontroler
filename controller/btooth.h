#ifndef BTOOTH_H
#define BTOOTH_H

#include <SoftwareSerial.h>

// Arduino pin map https://www.arduino.cc/en/Hacking/PinMapping168
// TX on the JY-MCU RX PIN, And vice-versa
#define BTOOTH_TX_PIN 5 // Digital pin 5 = atmega pin 6
#define BTOOTH_RX_PIN 4 // Digital pin 6 = atmega pin 11
#define BTOOTH_RATE 115200

SoftwareSerial btooth_serial(BTOOTH_RX_PIN, BTOOTH_TX_PIN);
String rcv_buf = ""; 

void setup_btooth();
String read_btooth_data();
void write_btooth_data(String data);

#endif
