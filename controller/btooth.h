/*
 * This file is part of BeerFridgeController.
 *
 *    BeerFridgeController is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    BeerFridgeController is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with BeerFridgeController.  If not, see <http://www.gnu.org/licenses/>.
 *    
 *    BeerFridgeController  Copyright (C) 2016-2017  Gabriel Tremblay
 *    
*/
#ifndef BTOOTH_H
#define BTOOTH_H

#include <SoftwareSerial.h>

// Arduino pin map https://www.arduino.cc/en/Hacking/PinMapping168
// TX on the JY-MCU RX PIN, And vice-versa
#define BTOOTH_TX_PIN 5 // Digital pin 5 = atmega pin 6
#define BTOOTH_RX_PIN 4 // Digital pin 6 = atmega pin 11
#define BTOOTH_RATE 57600  // Arduino limitation is 57600 in READ

SoftwareSerial btooth_serial(BTOOTH_RX_PIN, BTOOTH_TX_PIN);

void setup_btooth();
int read_btooth_data(char *buf, int bufsize);
void write_btooth_data(const char *data);

#endif
