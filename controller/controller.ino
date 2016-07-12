/*
 * Requires: extEEPROM
 */

#include <extEEPROM.h>
#include <Wire.h>

/*
 * External EEPROM
 * http://www.st.com/web/en/resource/technical/document/datasheet/DM00071904.pdf
*/
//extEEPROM eep(kbits_2, 1, 16);
//unsigned long test =0L;


void setup() {
  Serial.begin(115200);   // We'll send debugging information via the Serial monitor
}

void loop() {
   compute_flow_meters_ml();
   delay(1000); 
}
