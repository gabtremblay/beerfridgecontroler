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
#ifndef EEPROM_H
#define EEPROM_H
#include <extEEPROM.h>

#define FLOW1_ADDR 0
#define FLOW2_ADDR 32
#define FSR_EMPTY_ADDR 64
#define FSR_FULL_ADDR 96
#define FLOW_CAL 128

// Arduino pin map https://www.arduino.cc/en/Hacking/PinMapping168
extEEPROM eep(kbits_2, 1, 16);

struct EEPROM_VAUES {
  unsigned long flm1_cur_ml = 0;
  unsigned long flm2_cur_ml = 0;
  unsigned long fsr_empty_val = 0;
  unsigned long fsr_full_val = 0;
  float fl_cal = 0.0;
};

EEPROM_VAUES get_saved_eeprom_values();
void save_eeprom_values(EEPROM_VAUES values);
void save_flow1_value(unsigned long value);
void save_flow2_value(unsigned long value);
void save_fsr_empty_value(unsigned long value);
void save_fsr_full_value(unsigned long value);
void save_flow_cal_value(float value);

#endif

