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
 * Flow Meters
 */
#include "flow_meters.h"
   

void setup_flow_meters(float fl1_total_eeprom, float fl2_total_eeprom, float flow_calibration_factor) {
  pinMode(FLOW_METER_ONE_DATA_PIN, INPUT);
  digitalWrite(FLOW_METER_ONE_DATA_PIN, HIGH);
  
  pinMode(FLOW_METER_TWO_DATA_PIN, INPUT);
  digitalWrite(FLOW_METER_TWO_DATA_PIN, HIGH);

  // Init pulses
  flow_meter_one_pulses = 0;
  flow_meter_two_pulses = 0;
  flow_meter_one_rate = 0;
  flow_meter_two_rate = 0;
  old_time = 0;
  
  meters_status.fl1_total_ml = fl1_total_eeprom;
  meters_status.fl2_total_ml = fl2_total_eeprom;
  meters_status.fl1_rate_mlsec = 0;
  meters_status.fl2_rate_mlsec = 0;

  // Set calibration factor
  set_flow_meter_calibration(flow_calibration_factor);
  
  // Start
  attach_flow_interrupts();
}

void set_current_fl1_total_ml(unsigned long ml){
  meters_status.fl1_total_ml = ml;
}

void set_current_fl2_total_ml(unsigned long ml){
  meters_status.fl2_total_ml = ml;
}

void set_flow_meter_calibration(float val){
    calibration_factor = val;
}

/*
Insterrupt callbacks
 */
void flow_meter_one_count_pulses()
{
  flow_meter_one_pulses++;
}

void flow_meter_two_count_pulses()
{
  flow_meter_two_pulses++;
}

void attach_flow_interrupts(void){
  // Attach interrupts to both sensors
  attachInterrupt(FLOW_METER_ONE_INT_PIN, flow_meter_one_count_pulses, FALLING);
  attachInterrupt(FLOW_METER_TWO_INT_PIN, flow_meter_two_count_pulses, FALLING);
}

void detach_flow_interrupts(void){
  // detach interrupts of both sensors
  detachInterrupt(FLOW_METER_ONE_INT_PIN);
  detachInterrupt(FLOW_METER_TWO_INT_PIN);
}


// Flow computation
FLOW_METERS compute_flow_meters_ml(void)
{  
   // Only process counters once per second
   if((millis() - old_time) > 1000) 
   { 
    // Disable the interrupt while calculating flow rate
    detach_flow_interrupts();
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flow_meter_one_rate = ((1000.0 / (millis() - old_time)) * flow_meter_one_pulses) / calibration_factor;
    flow_meter_two_rate = ((1000.0 / (millis() - old_time)) * flow_meter_two_pulses) / calibration_factor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    old_time = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    meters_status.fl1_rate_mlsec = (flow_meter_one_rate / 60) * 1000;
    meters_status.fl2_rate_mlsec = (flow_meter_two_rate / 60) * 1000;


    // Sum everything
    meters_status.fl1_total_ml += meters_status.fl1_rate_mlsec ;
    meters_status.fl2_total_ml += meters_status.fl2_rate_mlsec ;

    // Reset the pulse counter so we can start incrementing again
    flow_meter_one_pulses = 0;
    flow_meter_two_pulses = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attach_flow_interrupts();
    
  }
  return meters_status;
}

