#ifndef FSR_H
#define FSR_H

#define FSR_ANALOG_PIN 0

struct FSR_READOUT {
  int raw_value = 0;
  int scaled_value = 0;
};

FSR_READOUT get_fsr_readout(void);

#endif

