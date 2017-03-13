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
#include "data.h"

RCVD_CMD parse_command(String rcvd_command){
  RCVD_CMD received_command;
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(rcvd_command);

  // Test if parse is sucessful
  if (!root.success()) {
    #ifdef DEBUG
      Serial.println("Parse error");
    #endif
    received_command.parse_success = false;
    return received_command;
  }

  // Test API KEY
  if(root["api_key"] != API_KEY){
    #ifdef DEBUG
      Serial.println("Wrong API KEY");
    #endif
    received_command.parse_success = false;
    return received_command;
  }

  received_command.parse_success = true;
  received_command.cmd = root["cmd"];
  received_command.value = root["value"];

  return received_command;
}

void format_reply(CMD_REPLY data, char *replybuf, int replybufsize){
  StaticJsonBuffer<32> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["cmd"] = data.cmd;
  root.printTo(replybuf, replybufsize);
}

void format_command(OUT_DATA data, char *cmdbuf, int cmdbufsize){
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  root["cmd"] = data.cmd;
  root["fl1_total"] = data.fl1_total_ml;
  root["fl1_rate"] = data.fl1_rate_mlsec;
  root["fl2_total"] = data.fl2_total_ml;
  root["fl2_rate"] = data.fl2_rate_mlsec;
  root["temp_c"] = data.temp_celcius;
  root["fsr_empty"] = data.fsr_empty_val;
  root["fsr_current"] = data.fsr_current_val;
  root["fsr_full"] = data.fsr_full_val;
  root["fl_cal"] = data.fl_cal_factor;

  root.printTo(cmdbuf, cmdbufsize);
}

