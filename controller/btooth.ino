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

String read_btooth_data(){
  if (btooth_serial.available()) {
    rcv_buf = "";
    while(btooth_serial.available()) {
      rcv_buf += (char)btooth_serial.read();
    }
    return rcv_buf;
  }
  return "";
}

void write_btooth_data(String data){
    btooth_serial.println(data);
}

