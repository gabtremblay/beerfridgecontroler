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
#define CALIBRATION_FACTOR 75.0

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

#endif
