/*
 * Requires: extEEPROM
 */
#include <Wire.h>
#include "flow_meters.h"
#include "eeprom.h"
#include "temp_lm35.h"
#include "fsr.h"

void setup() {
  Serial.begin(115200);   // We'll send debugging information via the Serial monitor

  // Load saved flow data
  setup_eeprom();
  
  EEPROM_VAUES saved_values;
  saved_values = get_saved_eeprom_values();
  
  // Setup flow meter reader
  setup_flow_meters(saved_values.flm1_cur_ml, saved_values.flm2_cur_ml);
}

void loop() {
   FLOW_METERS meters_status;
   meters_status = compute_flow_meters_ml();

   // Save to eeprom
   EEPROM_VAUES current_values;
   current_values.flm1_cur_ml = meters_status.fl1_total_ml;
   current_values.flm2_cur_ml = meters_status.fl2_total_ml;
   save_eeprom_values(current_values);

   // Read temp
   float current_temp;
   current_temp = get_temp_celcius();

   // Get FSR Value
   FSR_READOUT fsr_readout;
   fsr_readout = get_fsr_readout();

   Serial.print("Analog reading = ");
   Serial.println(fsr_readout.raw_value);
   Serial.print("Adjusted: ");
   Serial.print(fsr_readout.scaled_value);
   Serial.println("%");

   Serial.print("Temperature (C): ");
   Serial.println(current_temp);
   
   // Print the flow rate for this second in litres / minute
   Serial.print("Flow One, ");
   // Print the number of litres flowed in this second
   Serial.print("  Current Liquid Flowing: ");             // Output separator
   Serial.print(meters_status.fl1_rate_mlsec);
   Serial.print("mL/Sec");
   Serial.print(",  Total:  ");
   Serial.print(meters_status.fl1_total_ml);
   Serial.println("mL");
    
   // Print the flow rate for this second in litres / minute
   Serial.print("Flow Two, ");
   Serial.print("  Current Liquid Flowing: ");             // Output separator
   Serial.print(meters_status.fl2_rate_mlsec);
   Serial.print("mL/Sec");
   Serial.print(",  Total:  ");
   Serial.print(meters_status.fl2_total_ml);
   Serial.println("mL");
   
   delay(1000); 
}
