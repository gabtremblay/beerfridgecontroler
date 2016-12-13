#include "fsr.h"

FSR_READOUT get_fsr_readout(void){
  FSR_READOUT readout;
  readout.raw_value = analogRead(FSR_ANALOG_PIN);
  readout.scaled_value = map(readout.raw_value, 0, 1023, 0, 255);
  return readout;
}


