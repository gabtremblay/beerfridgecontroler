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
// CO2 is not the same.
// We need "empty bottle value" and "full bottle weight"
// We save only when "FULL bottle Calibrated"
// Level is always calculated between those two deltas. No need to save values live since we
// get the weight at all time, not based on flow.
boolean fl1_pouring = false;
boolean fl2_pouring = false;


void setup() {
  Serial.begin(115200);   // We'll send debugging information via the Serial monitor

  // Load saved flow data
  setup_eeprom();
  
  saved_values = get_saved_eeprom_values();
  
  // Setup flow meter reader
  setup_flow_meters(saved_values.flm1_cur_ml, saved_values.flm2_cur_ml);

  // Setup bluetooth comms
  setup_btooth();

  // Delay a bit so everything can come up (Serial btooth)
  delay(1000);

  // Run initial readings so we have data
  // This is necessary since our read timer wont read before 1 sec.
  read_fridge_data();

  // We use timer so we can fine tune the sensor reads and bt comms.
  // Otherwise it could be all sync in the loop()
  // We read the data from the fridge ~1000ms
  read_fridge_data_timer.setInterval(1000, read_fridge_data);

  // Comms should have a reasonable lag. ~1s
  communicate_timer.setInterval(500, communicate);

}

void reset_eeprom(){
  // Reset all eeprom values to 0 
  // Use for first boot or for new eeprom
  save_flow1_value(0);
  save_flow2_value(0);
  save_fsr_empty_value(0);
  save_fsr_full_value(0);
  set_current_fl1_total_ml(0);
  set_current_fl2_total_ml(0);
  saved_values.fsr_empty_val = 0;
  saved_values.fsr_full_val = 0;
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
   // print_debug();
  #endif
}

void generate_status_cmd(char *buf, int bufsize){
  OUT_DATA data;
  char set_cmd[] = "SET";
  
  // Generate output data
  data.cmd = set_cmd;
  data.fl1_total_ml = current_meters_status.fl1_total_ml;
  data.fl1_rate_mlsec = current_meters_status.fl1_rate_mlsec;
  data.fl2_total_ml = current_meters_status.fl2_total_ml;
  data.fl2_rate_mlsec = current_meters_status.fl2_rate_mlsec;
  data.temp_celcius = current_temp;
  data.fsr_empty_val = saved_values.fsr_empty_val;
  data.fsr_current_val = current_fsr_readout.raw_value;
  data.fsr_full_val = saved_values.fsr_full_val;
  
  // We use a stack allocated buffer to reduce memory usage
  // the kind of str we're dealing with is kind of too big for the little arduino
  format_command(data, buf, bufsize);
}

void generate_reply_cmd(char *cmd, char *buf, int bufsize){
  CMD_REPLY data;
  // Generate output data
  data.cmd = cmd;
  format_reply(data, buf, bufsize);
}

void communicate() {
  char err_cmd[] = "ERR";
  char ok_cmd[] = "OK";
  char combuf[200];
  char reply_buf[32];
  
  generate_status_cmd(combuf, sizeof(combuf));
  write_btooth_data(combuf);

  // Read incoming command
  memset(combuf, 0, sizeof(combuf));
  int recv_size = read_btooth_data(combuf, sizeof(combuf));

  // Empty command, could be a fluke
  if(recv_size <= 0){
    return;
  }
  
  #ifdef DEBUG
    Serial.println(combuf);
  #endif
  
  RCVD_CMD rcvd_cmd = parse_command(combuf);
  
  if(!rcvd_cmd.parse_success){
    generate_reply_cmd(err_cmd, reply_buf, sizeof(reply_buf));
    write_btooth_data(reply_buf);
    return;
  } 

  #ifdef DEBUG
    Serial.println(rcvd_cmd.cmd);
  #endif
  
  if(String(rcvd_cmd.cmd) == "rst"){
    #ifdef DEBUG
      Serial.println("Resetting everything");
    #endif
    reset_eeprom();
  } else if (String(rcvd_cmd.cmd) == "set_fl1") {
    #ifdef DEBUG
      Serial.println(F("SET fl1"));
    #endif
    save_flow1_value(rcvd_cmd.value);
    set_current_fl1_total_ml(rcvd_cmd.value);
  } else if (String(rcvd_cmd.cmd) == "set_fl2") {
    #ifdef DEBUG
      Serial.println(F("SET fl2"));
    #endif
    save_flow2_value(rcvd_cmd.value);
    set_current_fl2_total_ml(rcvd_cmd.value);
  } else if (String(rcvd_cmd.cmd) == "rec_fsr_empty") {
    #ifdef DEBUG
      Serial.println(F("Rec empty fsr"));
    #endif
    saved_values.fsr_empty_val = current_fsr_readout.raw_value;
    save_fsr_empty_value(current_fsr_readout.raw_value);
  } else if (String(rcvd_cmd.cmd) == "rec_fsr_full") {
    #ifdef DEBUG
      Serial.println(F("Rec full FSR"));
    #endif
    saved_values.fsr_full_val = current_fsr_readout.raw_value;
    save_fsr_full_value(current_fsr_readout.raw_value);
  } else {
    #ifdef DEBUG
      Serial.println(F("Unknown command"));
    #endif
    generate_reply_cmd(err_cmd, reply_buf, sizeof(reply_buf));
    write_btooth_data(reply_buf);
    return;
  }
  
  // Generate a command "OK"
  generate_reply_cmd(ok_cmd, reply_buf, sizeof(reply_buf));
  write_btooth_data(reply_buf);
}

#ifdef DEBUG
void print_debug() {
   Serial.print(F("Analog reading = "));
   Serial.println(current_fsr_readout.raw_value);
   Serial.print(F("Adjusted: "));
   Serial.print(current_fsr_readout.scaled_value);
   Serial.println(F("%"));

   Serial.print(F("Temperature (C): "));
   Serial.println(current_temp);
   
   // Print the flow rate for this second in litres / minute
   Serial.print(F("Flow One, "));
   // Print the number of litres flowed in this second
   Serial.print(F("  Current Liquid Flowing: "));             // Output separator
   Serial.print(current_meters_status.fl1_rate_mlsec);
   Serial.print(F("mL/Sec"));
   Serial.print(F(",  Total:  "));
   Serial.print(current_meters_status.fl1_total_ml);
   Serial.println(F("mL"));
    
   // Print the flow rate for this second in litres / minute
   Serial.print(F("Flow Two, "));
   Serial.print(F("  Current Liquid Flowing: "));             // Output separator
   Serial.print(current_meters_status.fl2_rate_mlsec);
   Serial.print(F("mL/Sec"));
   Serial.print(F(",  Total:  "));
   Serial.print(current_meters_status.fl2_total_ml);
   Serial.println(F("mL"));
}
#endif
