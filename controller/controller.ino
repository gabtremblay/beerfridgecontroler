/*
 * Requires external library: extEEPROM, ArduinoJson
 */
 // Arduino pro / pro mini
 // 328 5.5v
 // 
 // EEprom save condition are triggered after each pour or when the user sets
 // The CO2 level base or isert a new tank.
 
 // Todo : Comms
 // Debug output
 // Output
 //  - fl1 rate, fl2 rate, fl1tot, fl2tot, temp, weight,
 // Input
 // Cmd : Set fl1 tot
 // Cmd : Set fl2 tot
 // Cmd : Set weight_0
 // CMd : Set weight_full


#include <Wire.h>
#include "flow_meters.h"
#include "eeprom.h"
#include "temp_lm35.h"
#include "fsr.h"
#include "btooth.h"
#include "SimpleTimer.h"

#define DEBUG true

// State vars
FLOW_METERS current_meters_status;
float current_temp;
FSR_READOUT current_fsr_readout;
  
// Run timers
SimpleTimer read_fridge_data_timer;
SimpleTimer communicate_timer;

// State change detection
boolean fl1_pouring = false;
boolean fl2_pouring = false;

// CO2 is not the same.
// We need "empty bottle value" and "full bottle weight"
// We save only when "FULL bottle inserted"
// Level is always calculated between those two deltas. No need to save values live since we
// get the weight at all time, not based on flow.

// Allow for a first run command: Set everything to 0;

void setup() {
  Serial.begin(115200);   // We'll send debugging information via the Serial monitor

  // Load saved flow data
  setup_eeprom();
  
  EEPROM_VAUES saved_values;
  saved_values = get_saved_eeprom_values();
  
  // Setup flow meter reader
  setup_flow_meters(saved_values.flm1_cur_ml, saved_values.flm2_cur_ml);

  // Setup bluetooth comms
  setup_btooth();
  
  // We read the data from the fridge more often to get more precision ~1000ms
  read_fridge_data_timer.setInterval(1000, read_fridge_data);

  // Comms should have a reasonable lag. ~1s
  communicate_timer.setInterval(1000, communicate);

}

void loop(){
  read_fridge_data_timer.run();
  communicate_timer.run();
}

void read_fridge_data() {
  // Read Flow meter data
  current_meters_status = compute_flow_meters_ml();
  
  // Read temp
  current_temp = get_temp_celcius();

  // Get FSR Value
  current_fsr_readout = get_fsr_readout();
  
  // Check if it's time to save data
  // Flow meter 1
  if (current_meters_status.fl1_rate_mlsec > 0 && fl1_pouring == false){
    fl1_pouring = true;
    Serial.println("Flow 1 pouring!");
  } else if (current_meters_status.fl1_rate_mlsec <= 0 && fl1_pouring == true){
    Serial.println("Flow 1 end of pour, we should save");
    fl1_pouring = false;
    
    // Create save data
    save_flow1_value(current_meters_status.fl1_total_ml);
  }

  // Flow meter 2
  if (current_meters_status.fl2_rate_mlsec > 0 && fl2_pouring == false){
    fl2_pouring = true;
    Serial.println("Flow 2 pouring!");
  } else if (current_meters_status.fl2_rate_mlsec <= 0 && fl2_pouring == true){
    Serial.println("Flow 2 end of pour, we should save");
    fl2_pouring = false;
    
    // Create save data
    save_flow2_value(current_meters_status.fl2_total_ml);
  }

  #ifdef DEBUG
    print_debug();
  #endif
}

void communicate() {
//Serial.println("Comms made");
}


void print_debug() {
   current_meters_status = compute_flow_meters_ml();

   Serial.print("Analog reading = ");
   Serial.println(current_fsr_readout.raw_value);
   Serial.print("Adjusted: ");
   Serial.print(current_fsr_readout.scaled_value);
   Serial.println("%");

   Serial.print("Temperature (C): ");
   Serial.println(current_temp);
   
   // Print the flow rate for this second in litres / minute
   Serial.print("Flow One, ");
   // Print the number of litres flowed in this second
   Serial.print("  Current Liquid Flowing: ");             // Output separator
   Serial.print(current_meters_status.fl1_rate_mlsec);
   Serial.print("mL/Sec");
   Serial.print(",  Total:  ");
   Serial.print(current_meters_status.fl1_total_ml);
   Serial.println("mL");
    
   // Print the flow rate for this second in litres / minute
   Serial.print("Flow Two, ");
   Serial.print("  Current Liquid Flowing: ");             // Output separator
   Serial.print(current_meters_status.fl2_rate_mlsec);
   Serial.print("mL/Sec");
   Serial.print(",  Total:  ");
   Serial.print(current_meters_status.fl2_total_ml);
   Serial.println("mL");

   // Btooth data:
   String btooth_data1 = "";//read_btooth_data();
   write_btooth_data("Allo");
   btooth_data1 = read_btooth_data();
   Serial.println(btooth_data1);
}

