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
#ifndef FLOW_METERS_H
#define FLOW_METERS_H

// Arduino pin map https://www.arduino.cc/en/Hacking/PinMapping168
#define FLOW_METER_ONE_DATA_PIN 2
#define FLOW_METER_ONE_INT_PIN 0  // digital pin 0 = atmega pin 2
#define FLOW_METER_TWO_DATA_PIN 3
#define FLOW_METER_TWO_INT_PIN 1  // digital pin 1 = atmega pin 3

// The hall-effect flow sensor outputs approximately 4 pulses per second per litre/minute of flow.
// Adjust to your own flow meter values
// Could vary between 1 and 100 depending on sensor model
float calibration_factor;

// Serial resettable values, used for counting purposes
// Should be loaded and saved to eeprom
struct FLOW_METERS {
  unsigned long fl1_total_ml = 0;
  unsigned long fl1_rate_mlsec = 0;
  unsigned long fl2_total_ml = 0;
  unsigned long fl2_rate_mlsec = 0;
};

// State vars
volatile int flow_meter_one_pulses;  // volatile since it will be incremented by interrupt
volatile int flow_meter_two_pulses;  // volatile since it will be incremented by interrupt
float flow_meter_one_rate;
float flow_meter_two_rate;

// Time keeping for accurate measure
unsigned long old_time;

// State business object
FLOW_METERS meters_status;

FLOW_METERS compute_flow_meters_ml(void);
void set_current_fl1_total_ml(unsigned long ml);
void set_current_fl2_total_ml(unsigned long ml);


#endif
