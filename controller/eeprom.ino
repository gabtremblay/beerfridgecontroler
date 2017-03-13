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
#include "eeprom.h" 

void setup_eeprom(){
  uint8_t eepStatus = eep.begin(twiClock400kHz); 
  
  if (eepStatus) {
      Serial.print(F("EEprom.begin() failed, status = "));
      Serial.println(eepStatus);
      while (1);
  }
  Serial.println(F("EEprom init success"));

}

// Get the ml values from eeprom
EEPROM_VAUES get_saved_eeprom_values() {
  EEPROM_VAUES current_values;
  current_values.flm1_cur_ml = EEPROMReadlong(FLOW1_ADDR, eep);
  current_values.flm2_cur_ml = EEPROMReadlong(FLOW2_ADDR, eep);
  current_values.fsr_empty_val = EEPROMReadlong(FSR_EMPTY_ADDR, eep);
  current_values.fsr_full_val = EEPROMReadlong(FSR_FULL_ADDR, eep);

  // Restore flow calibration
  current_values.fl_cal = (float)(EEPROMReadlong(FLOW_CAL, eep) / 100.0);
  return current_values;
}

// Save bulk values to eeprom 
void save_eeprom_values(EEPROM_VAUES current_values){
  save_flow1_value(current_values.flm1_cur_ml);
  save_flow2_value(current_values.flm2_cur_ml);
  save_fsr_empty_value(current_values.fsr_empty_val);
  save_fsr_full_value(current_values.fsr_full_val);
  save_flow_cal_value(current_values.fl_cal);
}

void save_flow1_value(unsigned long value){
  EEPROMWritelong(FLOW1_ADDR, value, eep);
}

void save_flow2_value(unsigned long value){
  EEPROMWritelong(FLOW2_ADDR, value, eep);
}

void save_fsr_empty_value(unsigned long value){
  EEPROMWritelong(FSR_EMPTY_ADDR, value, eep);
}

void save_fsr_full_value(unsigned long value){
  EEPROMWritelong(FSR_FULL_ADDR, value, eep);
}

// Flow cal is saved as long. divided to get a float.
void save_flow_cal_value(float value){
  unsigned long to_save = (unsigned long)(value * 100);
  EEPROMWritelong(FLOW_CAL, to_save, eep);
}

void EEPROMWritelong(int address, unsigned long value, extEEPROM eeprom)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  //Write the 4 bytes into the eeprom memory.
  eeprom.write(address, four);
  eeprom.write(address + 1, three);
  eeprom.write(address + 2, two);
  eeprom.write(address + 3, one);
}

unsigned long EEPROMReadlong(long address, extEEPROM eeprom)
{
  //Read the 4 bytes from the eeprom memory.
  unsigned long four = eeprom.read(address);
  unsigned long three = eeprom.read(address + 1);
  unsigned long two = eeprom.read(address + 2);
  unsigned long one = eeprom.read(address + 3);
  
  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

