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
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(rcvd_command);

  // Test if parse is sucessful
  if (!root.success()) {
    received_command.parse_success = false;
    return received_command;
  }

  // Test API KEY
  if(root["api_key"] != API_KEY){
    received_command.parse_success = false;
    return received_command;
  }

  received_command.cmd = root["cmd"];
  received_command.value = root["value"];

  return received_command;
}


String format_command(OUT_DATA data){
  String output;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  root["cmd"] = data.cmd;
  root["fl1_total_ml"] = data.fl1_total_ml;
  root["fl1_rate_mlsec"] = data.fl1_rate_mlsec;
  root["fl2_total_ml"] = data.fl2_total_ml;
  root["fl2_rate_mlsec"] = data.fl2_rate_mlsec;
  root["temp_celcius"] = data.temp_celcius;
  root["fsr_empty_val"] = data.fsr_empty_val;
  root["fsr_current_val"] = data.fsr_current_val;
  root["fsr_full_val"] = data.fsr_full_val;
  
  root.printTo(output);
  return output;
}

