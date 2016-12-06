#include "temp_lm35.h"

float get_temp_celcius(){
   return (5.0 * analogRead(TEMP_ANALOG_PIN) * 100.0) / 1024;
}

