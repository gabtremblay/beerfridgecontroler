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
#include "btooth.h"

void setup_btooth(){
  btooth_serial.begin(BTOOTH_RATE);
}

void read_btooth_data(char *buf, int bufsize){
  int rcursor = 0;
  if (btooth_serial.available()) {
    while(btooth_serial.available() && rcursor < bufsize) {
      buf[rcursor++] = btooth_serial.read();
    }
  }
  // Make sure we terminate the string
  buf[rcursor] = '\0';
}

void write_btooth_data(const char *data){
  btooth_serial.println(data);
}

