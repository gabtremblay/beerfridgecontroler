/*
 * Flow Meters
 */
byte FLOW_METER_ONE_DATA_PIN = 2;
byte FLOW_METER_ONE_INT_PIN = 0;  // 0 = digital pin 2

byte FLOW_METER_TWO_DATA_PIN = 3;
byte FLOW_METER_TWO_INT_PIN = 1;  // 1 = digital pin 3

// State vars
volatile int flow_meter_one_pulses;  // volatile since it will be incremented by interrupt
float flow_meter_one_rate;
unsigned long flow_meter_one_ml;

volatile int flow_meter_two_pulses;  // volatile since it will be incremented by interrupt
float flow_meter_two_rate;
unsigned long flow_meter_two_ml;

// Time keeping for accurate measure
unsigned long old_time;

// The hall-effect flow sensor outputs approximately 4 pulses per second per litre/minute of flow.
// Adjust to your own flow meter values
// Could vary between 1 and 100 depending on sensor model
float CALIBRATION_FACTOR = 75.0;


// Serial resettable values, used for counting purposes
// Should be loaded and saved to eeprom
unsigned long flow_meter_one_ml_total;
unsigned long flow_meter_two_ml_total;


// DEBUG
unsigned long flow_meter_test_start;
unsigned long flow_meter_test_took;
boolean flow_test_started = false;
boolean flow_test_ended = false;


void setup_flow_meters(void) {
  pinMode(FLOW_METER_ONE_DATA_PIN, INPUT);
  digitalWrite(FLOW_METER_ONE_DATA_PIN, HIGH);
  
  pinMode(FLOW_METER_TWO_DATA_PIN, INPUT);
  digitalWrite(FLOW_METER_TWO_DATA_PIN, HIGH);
  
  flow_meter_one_pulses = 0;
  flow_meter_two_pulses = 0;
  flow_meter_one_rate = 0.0;
  flow_meter_one_ml   = 0;
  flow_meter_two_rate = 0.0;
  flow_meter_two_ml   = 0;

  old_time = 0;
  
  // TAKE FROM EEPROM
  flow_meter_one_ml_total = 0;
  flow_meter_two_ml_total = 0;

  // debug
  flow_meter_test_start = 0;
  flow_meter_test_took = 0;
  attach_flow_interrupts();
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
void compute_flow_meters_ml(void)
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
    flow_meter_one_rate = ((1000.0 / (millis() - old_time)) * flow_meter_one_pulses) / CALIBRATION_FACTOR;
    flow_meter_two_rate = ((1000.0 / (millis() - old_time)) * flow_meter_two_pulses) / CALIBRATION_FACTOR;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    old_time = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flow_meter_one_ml = (flow_meter_one_rate / 60) * 1000;
    flow_meter_two_ml = (flow_meter_two_rate / 60) * 1000;


    // Sum everything
    flow_meter_one_ml_total += flow_meter_one_ml;
    flow_meter_two_ml_total += flow_meter_two_ml;

    // DEBUG
    if(flow_meter_one_ml_total > 0 and flow_test_started == false){
      flow_test_started = true;
      Serial.println("Flow test started");
      flow_meter_test_start = millis();
    }

    if(flow_test_started and flow_meter_one_ml_total >= 1000){
      if(flow_test_ended == false){
        flow_meter_test_took = millis() - flow_meter_test_start;
        Serial.print("Flowing One Liter took: ");
        Serial.print(flow_meter_test_took);
        Serial.println("ms");
        flow_test_ended = true;
      }
    }
  
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow One rate: ");
    Serial.print(int(flow_meter_one_rate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flow_meter_one_ml);
    Serial.print("mL/Sec");
    Serial.print(",  Total:  ");
    Serial.print(flow_meter_one_ml_total);
    Serial.println("mL");
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow Two rate: ");
    Serial.print(int(flow_meter_two_rate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flow_meter_two_ml);
    Serial.print("mL/Sec");
    Serial.print(",  Total:  ");
    Serial.print(flow_meter_two_ml_total);
    Serial.println("mL");

    // Reset the pulse counter so we can start incrementing again
    flow_meter_one_pulses = 0;
    flow_meter_two_pulses = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attach_flow_interrupts();
  }
}

