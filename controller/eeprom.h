#ifndef EEPROM_H
#define EEPROM_H
#include <extEEPROM.h>

#define FLOW1_ADDR 0
#define FLOW2_ADDR 16

extEEPROM eep(kbits_2, 1, 16);

struct EEPROM_VAUES {
  unsigned long flm1_cur_ml;
  unsigned long flm2_cur_ml;
};

EEPROM_VAUES get_saved_eeprom_values();
void save_eeprom_values(EEPROM_VAUES values);

#endif

