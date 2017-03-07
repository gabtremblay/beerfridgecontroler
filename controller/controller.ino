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
 
 /*
 * Requires external library: extEEPROM, ArduinoJson
 */
 // Arduino pro / pro mini
 // 328 5.5v
 // 
 // EEprom save condition are triggered after each pour or when the user sets
 // the CO2 level base or isert a new tank.
 
 // Todo : Finish comms, Implement incoming commands
// Bug: "cmd":"SET","fl1_total_ml":1098,"fl1_rate_mlsec":0,"fl2_total_ml":1405,"fl2_rate_mlsec":0,"temp_celcius":.93,"fsr_empty_val":,"fsr_current_":,":}



#include <Wire.h>
#include "flow_meters.h"
#include "eeprom.h"
#include "temp_lm35.h"
#include "fsr.h"
#include "btooth.h"
#include "SimpleTimer.h"
#include "data.h"

#define DEBUG true

// State vars
FLOW_METERS current_meters_status;
float current_temp;
EEPROM_VAUES saved_values;
FSR_READOUT current_fsr_readout;

// Run timers
SimpleTimer read_fridge_data_timer;
SimpleTimer communicate_timer;

// State change detection
boolean fl1_pouring = false;
boolean fl2_pouring = false;

// CO2 is not the same.
// We need "empty bottle value" and "full bottle weight"
// We save only when "FULL bottle Calibrated"
// Level is always calculated between those two deltas. No need to save values live since we
// get the weight at all time, not based on flow.

void setup() {
  Serial.begin(115200);   // We'll send debugging information via the Serial monitor

  // Load saved flow data
  setup_eeprom();
  
  saved_values = get_saved_eeprom_values();
  
  // Setup flow meter reader
  setup_flow_meters(saved_values.flm1_cur_ml, saved_values.flm2_cur_ml);

  // Setup bluetooth comms
  setup_btooth();

  // Run initial readings so we have data
  // This is necessary since our read timer wont read before 1 sec.
  read_fridge_data();

  // We use timer so we can fine tune the sensor reads and bt comms.
  // Otherwise it could be all sync in the loop()
  // We read the data from the fridge ~1000ms
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
  } else if (current_meters_status.fl1_rate_mlsec <= 0 && fl1_pouring == true){
    fl1_pouring = false;
    // Create save data
    save_flow1_value(current_meters_status.fl1_total_ml);
  }

  // Flow meter 2
  if (current_meters_status.fl2_rate_mlsec > 0 && fl2_pouring == false){
    fl2_pouring = true;
  } else if (current_meters_status.fl2_rate_mlsec <= 0 && fl2_pouring == true){
    fl2_pouring = false;
    // Create save data
    save_flow2_value(current_meters_status.fl2_total_ml);
  }

  #ifdef DEBUG
    print_debug();
  #endif
}

void communicate() {
  // Generate output data
  OUT_DATA data;
  data.cmd = "SET";
  data.fl1_total_ml = current_meters_status.fl1_total_ml;
  data.fl1_rate_mlsec = current_meters_status.fl1_rate_mlsec;
  data.fl2_total_ml = current_meters_status.fl2_total_ml;
  data.fl2_rate_mlsec = current_meters_status.fl2_rate_mlsec;
  data.temp_celcius = current_temp;
  data.fsr_empty_val = saved_values.fsr_empty_val;
  data.fsr_current_val = current_fsr_readout.raw_value;
  data.fsr_full_val = saved_values.fsr_full_val;

  String json_str = format_command(data);
  Serial.println(json_str);
  write_btooth_data(json_str);
  
  // Fill data for output
  // Write to bluetooth

     // Btooth data:
   //String btooth_data1 = "";//read_btooth_data();
   //write_btooth_data("Allo");
   //btooth_data1 = read_btooth_data();
   //Serial.println(btooth_data1);
//Serial.println("Comms made");
}


void print_debug() {
   //current_meters_status = compute_flow_meters_ml();

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
}

