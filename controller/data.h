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
 * Data communication management
 * Input format
 * {"api_key": "key", "cmd":"cmd_str", "val": val_long}
 * 
 * Input Commands
 * (Val can be null)
 * "init_eeprom" : null -> init all epprom values to 0 (new eeprom inserted / full reset)
 * "set_fl1" : 0.00 -> Set fl1 tot ml
 * "set_fl2" : 0.00 -> Set fl2 tot ml
 * "rec_fsr_0" : null -> Record fsr value when bottle is empty (Calibration)
 * "rec_fsr_100" : null -> Record fsr value when bottle is full (Calibration)
 * 
 * Output
 * 
 */
#ifndef DATA_H
#define DATA_H
#include <ArduinoJson.h>

// Change this key so no rogue connected phone can fsck the system.
// AKA "drunk friend protection"
#define API_KEY "2a5e0eec"

// cmd: in case of ok or err, all values will be 0
// Possible values : "SET" (default), 
struct OUT_DATA {
  char *cmd;                      
  unsigned long fl1_total_ml = 0;    // Total ml that flowed trough flow meter #1 since reset
  unsigned long fl1_rate_mlsec = 0;  // Current flow rate of flow meter #1
  unsigned long fl2_total_ml = 0;    // Total ml that flowed trough flow meter #2 since reset
  unsigned long fl2_rate_mlsec = 0;  // Current flow rate of flow meter #2
  float temp_celcius = 0.0;          // Current internal temp in celcius
  unsigned long fsr_empty_val = 0;   // FSR value when bottle is empty
  unsigned long fsr_current_val = 0; // Current FSR reading
  unsigned long fsr_full_val = 0;    // FSR value when bottle is full
};

// Replies "OK", "ERR"
struct CMD_REPLY {
  char *cmd;
};

// recv should be in the format of
// {"api_key":"", "cmd":"cmd_name", "value": 0}
// Value can be null
// CMDS:
// rst : set all eeprom values to 0  (null val)
// set_fl1 : Set flow meter 1 total ml
// set_fl2 : Set flow meter 2 total ml 
// rec_fsr_empty: Record fsr value as empty (null val)
// rec_fsr_full: Record fsr value as full (null val)

struct RCVD_CMD {
  boolean parse_success = true;
  const char* cmd;
  unsigned long value;
};

RCVD_CMD parse_command(String rcvd_command);
void format_command(OUT_DATA data, char *cmdbuf, int cmdbufsize);
void format_reply(CMD_REPLY data, char *replybuf, int replybufsize);

#endif
