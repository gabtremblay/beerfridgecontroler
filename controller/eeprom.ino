#include "eeprom.h"

void setup_eeprom(){
  uint8_t eepStatus = eep.begin(twiClock400kHz); 
  
  if (eepStatus) {
      Serial.print("EEprom.begin() failed, status = ");
      Serial.println(eepStatus);
      while (1);
  }
  Serial.println("EEprom init success");

}

// Get the ml values from eeprom
EEPROM_VAUES get_saved_eeprom_values() {
  EEPROM_VAUES current_values;
  current_values.flm1_cur_ml = EEPROMReadlong(FLOW1_ADDR, eep);
  current_values.flm2_cur_ml = EEPROMReadlong(FLOW2_ADDR, eep);
  current_values.fsr_val = EEPROMReadlong(FSR_ADDR, eep);
  return current_values;
}

// Save values to eeprom
void save_eeprom_values(EEPROM_VAUES current_values){
  EEPROMWritelong(FLOW1_ADDR, current_values.flm1_cur_ml, eep);
  EEPROMWritelong(FLOW2_ADDR, current_values.flm2_cur_ml, eep);
  EEPROMWritelong(FSR_ADDR, current_values.fsr_val, eep);
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

