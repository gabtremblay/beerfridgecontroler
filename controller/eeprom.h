#ifndef EEPROM_H
#define EEPROM_H
#include <extEEPROM.h>

#define FLOW1_ADDR 0
#define FLOW2_ADDR 32
#define FSR_EMPTY_ADDR 64
#define FSR_FULL_ADDR 96

extEEPROM eep(kbits_2, 1, 16);

struct EEPROM_VAUES {
  unsigned long flm1_cur_ml = 0;
  unsigned long flm2_cur_ml = 0;
  unsigned long fsr_empty_val = 0;
  unsigned long fsr_full_val = 0;
};

EEPROM_VAUES get_saved_eeprom_values();
void save_eeprom_values(EEPROM_VAUES values);

#endif

